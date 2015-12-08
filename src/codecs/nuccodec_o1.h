//
// Copyright (c) 2015
// Leibniz Universitaet Hannover, Institut fuer Informationsverarbeitung (TNT)
// Contact: Jan Voges <voges@tnt.uni-hannover.de>
//

//
// This file is part of tsc.
//
// Tsc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tsc is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with tsc. If not, see <http://www.gnu.org/licenses/>
//

#ifndef TSC_NUCCODEC_O1_H
#define TSC_NUCCODEC_O1_H

#include "../tvclib/bbuf.h"
#include "../tvclib/cbufint64.h"
#include "../tvclib/cbufstr.h"
#include "../tvclib/str.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define WINDOW_SZ 10
#define ALPHA 0.5

// Encoder
// -----------------------------------------------------------------------------

typedef struct nucenc_t_ {
    size_t in_sz;       // Accumulated input size
    size_t rec_blk_cnt; // No. of records processed in the current block
    size_t rec_tot_cnt; // Total # of records processed
    bool   first;       // 'false', if first line has not been processed yet
    str_t  *tmp;        // Temporal storage for e.g. prediction residues
    str_t  *rname_prev; // Holding current RNAME

    // Statistics
    str_t  *stat_fname; // File name for statistics file
    FILE   *stat_fp;    // File pointer to statistics file
    size_t m_blk_cnt;
    size_t m_tot_cnt;
    size_t i_blk_cnt;
    size_t i_tot_cnt;
    double stogy_mu;  // Mean STOGY length
    double mod_mu;    // Mean MOD length
    double trail_mu;  // Mean TRAIL length

    // Circular buffers
    cbufint64_t *neo_cbuf;
    cbufint64_t *pos_cbuf;
    cbufstr_t   *exs_cbuf;
} nucenc_t;

nucenc_t * nucenc_new(void);
void nucenc_free(nucenc_t *nucenc);
void nucenc_add_record(nucenc_t       *nucenc,
                       const char     *rname,
                       const uint32_t pos,
                       const char     *cigar,
                       const char     *seq);
size_t nucenc_write_block(nucenc_t *nucenc, FILE *fp);

// Decoder
// -----------------------------------------------------------------------------

typedef struct nucdec_t_ {
    size_t out_sz; // Accumulated output size

    // Circular buffers
    cbufint64_t *neo_cbuf;
    cbufint64_t *pos_cbuf;
    cbufstr_t   *exs_cbuf;
} nucdec_t;

nucdec_t * nucdec_new(void);
void nucdec_free(nucdec_t *nucdec);
size_t nucdec_decode_block(nucdec_t *nucdec,
                           FILE     *fp,
                           str_t    **rname,
                           uint32_t *pos,
                           str_t    **cigar,
                           str_t    **seq);

#endif // TSC_NUCCODEC_O1_H

