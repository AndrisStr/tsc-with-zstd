/******************************************************************************
 * Copyright (c) 2015 Institut fuer Informationsverarbeitung (TNT)            *
 * Contact: Jan Voges <jvoges@tnt.uni-hannover.de>                            *
 *                                                                            *
 * This file is part of tsc.                                                  *
 ******************************************************************************/

#include "nuccodec.h"
#include "accodec.h"
#include "bbuf.h"
#include "frw.h"
#include "tsclib.h"
#include <ctype.h>
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>


/******************************************************************************
 * Encoder                                                                    *
 ******************************************************************************/
static void nucenc_init(nucenc_t* nucenc)
{
    nucenc->block_lc = 0;
}

nucenc_t* nucenc_new(void)
{
    nucenc_t* nucenc = (nucenc_t*)tsc_malloc_or_die(sizeof(nucenc_t));
    nucenc->pos_cbuf = cbufint64_new(NUCCODEC_WINDOW_SZ);
    nucenc->cigar_cbuf = cbufstr_new(NUCCODEC_WINDOW_SZ);
    nucenc->seq_cbuf = cbufstr_new(NUCCODEC_WINDOW_SZ);
    nucenc->exp_cbuf = cbufstr_new(NUCCODEC_WINDOW_SZ);
    nucenc->out_buf = str_new();
    nucenc_init(nucenc);
    return nucenc;
}

void nucenc_free(nucenc_t* nucenc)
{
    if (nucenc != NULL) {
        cbufint64_free(nucenc->pos_cbuf);
        cbufstr_free(nucenc->cigar_cbuf);
        cbufstr_free(nucenc->seq_cbuf);
        cbufstr_free(nucenc->exp_cbuf);
        str_free(nucenc->out_buf);
        free((void*)nucenc);
        nucenc = NULL;
    } else { /* nucenc == NULL */
        tsc_error("Tried to free NULL pointer.\n");
    }
}

static void nucenc_expand(str_t* exp, uint64_t pos, const char* cigar, const char* seq)
{
    str_clear(exp);

    size_t cigar_idx = 0;
    size_t cigar_len = strlen(cigar);
    size_t op_len = 0; /* length of current CIGAR operation */
    size_t seq_idx = 0;

    /* Iterate through CIGAR string and expand SEQ. */
    for (cigar_idx = 0; cigar_idx < cigar_len; cigar_idx++) {
        if (isdigit(cigar[cigar_idx])) {
            op_len = op_len * 10 + cigar[cigar_idx] - '0';
            continue;
        }

        switch (cigar[cigar_idx]) {
        case 'M':
        case '=':
        case 'X':
            str_append_cstrn(exp, &seq[seq_idx], op_len);
            seq_idx += op_len;
            break;
        case 'I':
        case 'S':
            str_append_cstrn(exp, &seq[seq_idx], op_len);
            seq_idx += op_len;
            break;
        case 'D':
        case 'N': {
            size_t i = 0;
            for (i = 0; i < op_len; i++) { str_append_char(exp, '_'); }
            break;
        }
        case 'H':
        case 'P':
            str_append_cstrn(exp, &seq[seq_idx], op_len);
            seq_idx += op_len;
            break;
        default: tsc_error("Bad CIGAR string: %s\n", cigar);
        }

        op_len = 0;
    }
    DEBUG("exp: %s", exp->s);
}

static void nucenc_diff(str_t* diff, str_t* exp, uint64_t exp_pos, str_t* ref, uint64_t ref_pos)
{
    /* Compute position offset. */
    uint64_t pos_off = exp_pos - ref_pos;

    /* Determine length of match. */
    if (pos_off > ref->n) tsc_error("Position offset too large: %d\n", pos_off);
    uint64_t match_len = ref->n - pos_off;
    if (exp->n + pos_off < match_len) match_len = exp->n + pos_off;
    
    /* Allocate memory for diff. */
    str_clear(diff);
    str_reserve(diff, exp->n);
    
    /* Compute differences from exp to ref. */
    size_t ref_idx = pos_off;
    size_t exp_idx = 0;
    int d = 0;
    size_t i = 0;
    for (i = pos_off; i < match_len; i++) {
        d = (int)exp->s[exp_idx++] - (int)ref->s[ref_idx++] + ((int)'T');
        str_append_char(diff, (char)d);
    }
    while (i < exp->n) str_append_char(diff, exp->s[i++]);

    DEBUG("pos_off: %"PRIu64"", pos_off);
    DEBUG("ref:  %s", ref->s);
    DEBUG("exp:  %s", exp->s);
    DEBUG("diff: %s", diff->s);
}

static double nucenc_entropy(str_t* seq)
{
    /* Make histogram. */
    unsigned int i = 0;
    unsigned int freq[256];
    memset(freq, 0, sizeof(freq));
    for (i = 0; i < seq->n; i++) freq[(int)(seq->s[i])]++;

    /* Calculate entropy. */
    double n = (double)seq->n;
    double h = 0.0;
    for (i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            double prob = (double)freq[i] / n;
            h -= prob * log2(prob);
        }
    }

    return h;
}

void nucenc_add_record(nucenc_t* nucenc, uint64_t pos, const char* cigar, const char* seq)
{    
    /* Allocate memory for encoding. */
    str_t* exp = str_new();      /* expanded current sequence */
    str_t* diff = str_new();     /* difference sequence */
    str_t* diff_min = str_new(); /* best difference sequence */

    if (cigar[0] != '*' && seq[0] != '*' && nucenc->block_lc > 0) {
        /* SEQ is mapped , SEQ & CIGAR are present and this is not the first
         * record in the current block. Encode it!
         */

        double entropy = 0;
        double entropy_min = DBL_MAX;

        /* Expand current record. */
        nucenc_expand(exp, pos, cigar, seq);

        /* Compute differences from EXP to every record in the buffer. */
        size_t cbuf_idx = 0;
        size_t cbuf_n = nucenc->pos_cbuf->n;
        //size_t cbuf_idx_min = 0;

        do {
            /* Get expanded reference sequence from buffer. */
            str_t* ref = cbufstr_get(nucenc->exp_cbuf, cbuf_idx);
            uint64_t ref_pos = cbufint64_get(nucenc->pos_cbuf, cbuf_idx);
            cbuf_idx++;

            /* Compute difference.s */
            nucenc_diff(diff, exp, pos, ref, ref_pos);

            /* Check the entropy of the difference. */
            entropy = nucenc_entropy(diff);
            if (entropy < entropy_min) {
                entropy_min = entropy;
                //cbuf_idx_min = cbuf_idx;
                str_copy_str(diff_min, diff);
            }
        } while (cbuf_idx < cbuf_n /* && entropy_curr > threshold */);

        /* Append to output stream. */
        /* TODO: Append cbuf_idx_min */
        str_append_str(nucenc->out_buf, diff_min);
        str_append_cstr(nucenc->out_buf, "\n");

        /* Push POS, CIGAR, SEQ and EXP to circular buffers. */
        cbufint64_push(nucenc->pos_cbuf, pos);
        cbufstr_push(nucenc->cigar_cbuf, cigar);
        cbufstr_push(nucenc->seq_cbuf, seq);
        cbufstr_push(nucenc->exp_cbuf, exp->s);

        str_clear(exp);
        str_clear(diff);
        str_clear(diff_min);

    } else {
        /* SEQ is not mapped or is not present, or CIGAR is not present or
         * this is the first sequence in the current block.
         */

        /* Output record. */
        if (pos > pow(10, 10) - 1) tsc_error("Buffer too small for POS data!\n");
        char tmp[101]; snprintf(tmp, sizeof(tmp), "%jd", pos);
        str_append_cstr(nucenc->out_buf, tmp);
        str_append_cstr(nucenc->out_buf, "\t");
        str_append_cstr(nucenc->out_buf, cigar);
        str_append_cstr(nucenc->out_buf, "\t");
        str_append_cstr(nucenc->out_buf, seq);
        str_append_cstr(nucenc->out_buf, "\n");

        /* If this record is present, add it to circular buffers. */
        if (cigar[0] != '*' && seq[0] != '*') {
            /* Expand current record. */
            nucenc_expand(exp, pos, cigar, seq);

            /* Push POS, CIGAR, SEQ and EXP to circular buffers. */
            cbufint64_push(nucenc->pos_cbuf, pos);
            cbufstr_push(nucenc->cigar_cbuf, cigar);
            cbufstr_push(nucenc->seq_cbuf, seq);
            cbufstr_push(nucenc->exp_cbuf, exp->s);
        }
    }

    str_free(exp);
    str_free(diff);
    str_free(diff_min);

    /*DEBUG("out_buf: \n%s", nucenc->out_buf->s);*/
    nucenc->block_lc++;
}

static void nucenc_reset(nucenc_t* nucenc)
{
    nucenc->block_lc = 0;
    cbufint64_clear(nucenc->pos_cbuf);
    cbufstr_clear(nucenc->cigar_cbuf);
    cbufstr_clear(nucenc->seq_cbuf);
    cbufstr_clear(nucenc->exp_cbuf);
    str_clear(nucenc->out_buf);
}

size_t nucenc_write_block(nucenc_t* nucenc, FILE* ofp)
{
    /* Write block:
     * - unsigned char[8]: "NUC-----"
     * - uint32_t        : no. of lines in block
     * - uint32_t        : block size
     * - unsigned char[] : data
     */
    size_t header_byte_cnt = 0;
    size_t data_byte_cnt = 0;
    size_t byte_cnt = 0;

    header_byte_cnt += fwrite_cstr(ofp, "NUC-----");
    header_byte_cnt += fwrite_uint32(ofp, nucenc->block_lc);

    /* Compress block with AC. */
    unsigned char* ac_in = (unsigned char*)nucenc->out_buf->s;
    unsigned int ac_in_sz = nucenc->out_buf->n;
    unsigned int ac_out_sz = 0;
    unsigned char* ac_out = arith_compress_O0(ac_in, ac_in_sz, &ac_out_sz);
    tsc_log("Compressed NUC block with AC: %zu bytes -> %zu bytes\n", ac_in_sz, ac_out_sz);

    /* Write compressed block to ofp. */
    header_byte_cnt += fwrite_uint32(ofp, ac_out_sz);
    data_byte_cnt += fwrite_buf(ofp, ac_out, ac_out_sz);
    free((void*)ac_out);

    /* Log this. */
    byte_cnt = header_byte_cnt + data_byte_cnt;
    tsc_log("Wrote NUC block: %zu bytes (header) + %zu bytes (data) = %zu bytes\n",
            header_byte_cnt, data_byte_cnt, byte_cnt);

    /* Reset encoder for next block. */
    nucenc_reset(nucenc);
    
    return byte_cnt;
}

/******************************************************************************
 * Decoder                                                                    *
 ******************************************************************************/
static void nucdec_init(nucdec_t* nucdec)
{
    nucdec->block_lc = 0;
}

nucdec_t* nucdec_new(void)
{
    nucdec_t* nucdec = (nucdec_t*)tsc_malloc_or_die(sizeof(nucdec_t));
    nucdec->pos_cbuf = cbufint64_new(NUCCODEC_WINDOW_SZ);
    nucdec->cigar_cbuf = cbufstr_new(NUCCODEC_WINDOW_SZ);
    nucdec->seq_cbuf = cbufstr_new(NUCCODEC_WINDOW_SZ);
    nucdec->exp_cbuf = cbufstr_new(NUCCODEC_WINDOW_SZ);
    nucdec_init(nucdec);
    return nucdec;
}

void nucdec_free(nucdec_t* nucdec)
{
    if (nucdec != NULL) {
        cbufint64_free(nucdec->pos_cbuf);
        cbufstr_free(nucdec->cigar_cbuf);
        cbufstr_free(nucdec->seq_cbuf);
        cbufstr_free(nucdec->exp_cbuf);
        free((void*)nucdec);
        nucdec = NULL;
    } else { /* nucdec == NULL */
        tsc_error("Tried to free NULL pointer.\n");
    }
}

static void nucdec_reset(nucdec_t* nucdec)
{
    nucdec->block_lc = 0;
    cbufint64_clear(nucdec->pos_cbuf);
    cbufstr_clear(nucdec->cigar_cbuf);
    cbufstr_clear(nucdec->seq_cbuf);
    cbufstr_clear(nucdec->exp_cbuf);
}

void nucdec_decode_block(nucdec_t* nucdec, FILE* ifp, uint64_t* pos, str_t** cigar, str_t** seq)
{
    /* Read block:
     * - unsigned char[8]: "NUC-----"
     * - uint32_t        : no. of lines in block
     * - uint32_t        : block size
     * - unsigned char[] : data
     */
    unsigned char block_id[8];
    uint32_t block_sz;

    if (fread_buf(ifp, block_id, sizeof(block_id)) != sizeof(block_id))
        tsc_error("Could not read block ID!\n");
    if (fread_uint32(ifp, &(nucdec->block_lc)) != sizeof(nucdec->block_lc))
        tsc_error("Could not read no. of lines in block!\n");
    if (fread_uint32(ifp, &block_sz) != sizeof(block_sz))
        tsc_error("Could not read block size!\n");
    if (strncmp("NUC-----", (const char*)block_id, sizeof(block_id)))
        tsc_error("Wrong block ID: %s\n", block_id);
    tsc_log("Reading NUC block: %"PRIu32" bytes in %"PRIu32" lines\n", block_sz, nucdec->block_lc);

    /* Read block. */
    bbuf_t* bbuf = bbuf_new();
    bbuf_reserve(bbuf, block_sz);
    fread_buf(ifp, bbuf->bytes, block_sz);

    /* Uncompress block with AC. */
    unsigned char* ac_in = bbuf->bytes;
    unsigned int ac_in_sz = block_sz;
    unsigned int ac_out_sz = 0;
    unsigned char* ac_out = arith_uncompress_O0(ac_in, ac_in_sz, &ac_out_sz);
    tsc_log("Decompressed NUC block with AC: %zu bytes -> %zu bytes\n", ac_in_sz, ac_out_sz);
    bbuf_free(bbuf);

    /* TODO: Decode block */


    free((void*)ac_out);

    /* Reset decoder for next block. */
    nucdec_reset(nucdec);
}
