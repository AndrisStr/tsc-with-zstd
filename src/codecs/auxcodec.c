/*
 * The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third
 * party and contributor rights, including patent rights, and no such
 * rights are granted under this license.
 *
 * Copyright (c) 2015-2016, Leibniz Universitaet Hannover, Institut fuer
 * Informationsverarbeitung (TNT)
 * Contact: <voges@tnt.uni-hannover.de>
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * Neither the name of the TNT nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//
// Aux block format:
//   unsigned char id[8]          :"aux----" + '\0'
//   uint64_t      record_cnt     : No. of lines in block
//   uint64_t      uncompressed_sz: Size of uncompressed data
//   uint64_t      compressed_sz  : Compressed data size
//   uint64_t      compressed_crc : CRC64 of compressed data
//   unsigned char *compressed    : Compressed data
//

#include "codecs/auxcodec.h"
#include "codecs/zlib_wrap.h"
#include "tsclib/fio.h"
#include "tsclib/log.h"
#include "tsclib/mem.h"
#include "support/crc64.h"
#include <inttypes.h>
#include <string.h>

static void auxcodec_init(auxcodec_t *auxcodec)
{
    auxcodec->record_cnt = 0;
    str_clear(auxcodec->uncompressed);
    if (auxcodec->compressed != NULL) free(auxcodec->compressed);
    auxcodec->compressed = NULL;
    auxcodec->compressed_sz = 0;
}

auxcodec_t * auxcodec_new(void)
{
    auxcodec_t *auxcodec = (auxcodec_t *)tsc_malloc(sizeof(auxcodec_t));
    auxcodec->uncompressed = str_new();
    auxcodec->compressed = NULL;
    auxcodec_init(auxcodec);
    return auxcodec;
}

void auxcodec_free(auxcodec_t *auxcodec)
{
    if (auxcodec != NULL) {
        str_free(auxcodec->uncompressed);
        if (auxcodec->compressed != NULL) {
            free(auxcodec->compressed);
            auxcodec->compressed = NULL;
        }
        free(auxcodec);
        auxcodec = NULL;
    } else {
        tsc_error("Tried to free null pointer\n");
    }
}

// Encoder methods
// -----------------------------------------------------------------------------

void auxcodec_add_record(auxcodec_t     *auxcodec,
                         const uint16_t flag,
                         const uint8_t  mapq,
                         const char     *opt)
{
    auxcodec->record_cnt++;

    char flag_cstr[101];
    char mapq_cstr[101];

    snprintf(flag_cstr, sizeof(flag_cstr), "%"PRIu16, flag);
    snprintf(mapq_cstr, sizeof(mapq_cstr), "%"PRIu8, mapq);

    str_append_cstr(auxcodec->uncompressed, flag_cstr);
    str_append_cstr(auxcodec->uncompressed, "\t");
    str_append_cstr(auxcodec->uncompressed, mapq_cstr);
    str_append_cstr(auxcodec->uncompressed, "\t");
    str_append_cstr(auxcodec->uncompressed, opt);
    str_append_cstr(auxcodec->uncompressed, "\n");
}

size_t auxcodec_write_block(auxcodec_t *auxcodec, FILE *fp)
{
    size_t ret = 0;

    // Compress block
    unsigned char *uncompressed = (unsigned char *)auxcodec->uncompressed->s;
    size_t uncompressed_sz = auxcodec->uncompressed->len;
    size_t compressed_sz = 0;
    unsigned char *compressed = zlib_compress(uncompressed, uncompressed_sz, &compressed_sz);

    // Compute CRC64
    uint64_t compressed_crc = crc64(compressed, compressed_sz);

    // Write compressed block
    unsigned char id[8] = "aux----"; id[7] = '\0';
    ret += tsc_fwrite_buf(fp, id, sizeof(id));
    ret += tsc_fwrite_uint64(fp, (uint64_t)auxcodec->record_cnt);
    ret += tsc_fwrite_uint64(fp, (uint64_t)uncompressed_sz);
    ret += tsc_fwrite_uint64(fp, (uint64_t)compressed_sz);
    ret += tsc_fwrite_uint64(fp, (uint64_t)compressed_crc);
    ret += tsc_fwrite_buf(fp, compressed, compressed_sz);

    // Free memory allocated by zlib_compress
    free(compressed);

    auxcodec_init(auxcodec);

    return ret;
}

// Decoder methods
// -----------------------------------------------------------------------------

static size_t auxcodec_decode(unsigned char *tmp,
                              size_t        tmp_sz,
                              uint16_t      *flag,
                              uint8_t       *mapq,
                              str_t         **opt)
{
    size_t ret = 0;
    size_t i = 0;
    size_t line = 0;
    unsigned char *cstr = tmp;
    unsigned int idx = 0;

    for (i = 0; i < tmp_sz; i++) {
        if (tmp[i] == '\n') {
            tmp[i] = '\0';
            str_append_cstr(opt[line], (const char *)cstr);
            ret += strlen((const char *)cstr);
            cstr = &tmp[i+1];
            idx = 0;
            line++;
            continue;
        }

        if (tmp[i] == '\t') {
            tmp[i] = '\0';
            switch (idx++) {
            case 0:
                flag[line] = (uint16_t)strtoul((const char *)cstr, NULL, 10);
                ret += sizeof(*flag);
                break;
            case 1:
                mapq[line] = (uint8_t)strtoul((const char *)cstr, NULL, 10);
                ret += sizeof(*mapq);
                break;
            case 2:
                // Fall through (all upcoming columns are 'opt')
            default:
                str_append_cstr(opt[line], (const char *)cstr);
                ret += strlen((const char *)cstr);
                str_append_char(opt[line], '\t');
                ret++;
                break;
            }
            cstr = &tmp[i+1];
        }
    }

    return ret;
}

size_t auxcodec_decode_block(auxcodec_t *auxcodec,
                            FILE        *fp,
                            uint16_t    *flag,
                            uint8_t     *mapq,
                            str_t       **opt)
{
    size_t ret = 0;

    unsigned char id[8];
    uint64_t      record_cnt;
    uint64_t      uncompressed_sz;
    uint64_t      compressed_sz;
    uint64_t      compressed_crc;
    unsigned char *compressed;

    // Read block
    ret += tsc_fread_buf(fp, id, sizeof(id));
    ret += tsc_fread_uint64(fp, &record_cnt);
    ret += tsc_fread_uint64(fp, &uncompressed_sz);
    ret += tsc_fread_uint64(fp, &compressed_sz);
    ret += tsc_fread_uint64(fp, &compressed_crc);
    compressed = (unsigned char *)tsc_malloc((size_t)compressed_sz);
    ret += tsc_fread_buf(fp, compressed, compressed_sz);

    // Check CRC64
    if (crc64(compressed, compressed_sz) != compressed_crc)
        tsc_error("CRC64 check failed for aux block\n");

    // Decompress block
    unsigned char *uncompressed = zlib_decompress(compressed, compressed_sz, uncompressed_sz);
    free(compressed);

    // Decode block
    auxcodec_decode(uncompressed, uncompressed_sz, flag, mapq, opt);
    free(uncompressed); // Free memory used for decoded bitstream

    auxcodec_init(auxcodec);

    return ret;
}

