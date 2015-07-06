/******************************************************************************
 * Copyright (c) 2015 Institut fuer Informationsverarbeitung (TNT)            *
 * Contact: Jan Voges <jvoges@tnt.uni-hannover.de>                            *
 *                                                                            *
 * This file is part of tsc.                                                  *
 ******************************************************************************/

#ifndef TSC_NUCCODEC_H
#define TSC_NUCCODEC_H

#include "bbuf.h"
#include "cbufint64.h"
#include "cbufstr.h"
#include "../str/str.h"
#include <stdint.h>
#include <stdio.h>

#define NUCCODEC_WINDOW_SZ 100 /* needed by o1 codec */

/******************************************************************************
 * Encoder                                                                    *
 ******************************************************************************/
typedef struct nucenc_t_ {
    size_t blkl_n;   /* no. of records processed in the curr block */

    /* Needed by o0 codec */
    str_t* residues; /* prediction residues (passed to the entropy coder) */

    /* Needed by o1 codec */
    size_t       diff_fail_n; /* no. of records where diff() didn't succeed */

    cbufint64_t* pos_cbuf;    /* circular buffer for POSitions          */
    cbufstr_t*   cigar_cbuf;  /* circular buffer for CIGARs             */
    cbufstr_t*   seq_cbuf;    /* circular buffer for SEQuences          */
    cbufstr_t*   exp_cbuf;    /* circular buffer for EXPanded sequences */

    bbuf_t*      pos_res;     /* position prediction residues */
    str_t*       cigar_res;   /* cigar prediction residues    */
    str_t*       seq_res;     /* sequence prediction residues */
} nucenc_t;

nucenc_t* nucenc_new(void);
void nucenc_free(nucenc_t* nucenc);
void nucenc_add_record(nucenc_t*      nucenc,
                       const int64_t  pos,
                       const char*    cigar,
                       const char*    seq);
size_t nucenc_write_block(nucenc_t* nucenc, FILE* ofp);

/******************************************************************************
 * Decoder                                                                    *
 ******************************************************************************/
typedef struct nucdec_t_ {

} nucdec_t;

nucdec_t* nucdec_new(void);
void nucdec_free(nucdec_t* nucdec);
void nucdec_decode_block(nucdec_t* nucdec,
                         FILE*     ifp,
                         int64_t*  pos,
                         str_t**   cigar,
                         str_t**   seq);

#endif /* TSC_NUCCODEC_H */

