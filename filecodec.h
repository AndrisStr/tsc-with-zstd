/*****************************************************************************
 * Copyright (c) 2015 Institut fuer Informationsverarbeitung (TNT)           *
 * Contact: Jan Voges <jvoges@tnt.uni-hannover.de>                           *
 *                                                                           *
 * This file is part of tsc.                                                 *
 *****************************************************************************/

#ifndef TSC_FILECODEC_H
#define TSC_FILECODEC_H

#include <stdio.h>
#include "samparser.h"
#include "seqcodec.h"
#include "qualcodec.h"
#include "auxcodec.h"

/*****************************************************************************
 * Encoder                                                                   *
 *****************************************************************************/
typedef struct fileenc_t_ {
    FILE*        ifp;
    FILE*        ofp;
    size_t       block_sz;
    samparser_t* samparser;
    seqenc_t*    seqenc;
    qualenc_t*   qualenc;
    auxenc_t*    auxenc;
} fileenc_t;

fileenc_t* fileenc_new(FILE* ifp, FILE* ofp, const size_t block_sz);
void fileenc_free(fileenc_t* fileenc);
void fileenc_encode(fileenc_t* fileenc);

/*****************************************************************************
 * Decoder                                                                   *
 *****************************************************************************/
typedef struct filedec_t_ {
    FILE*      ifp;
    FILE*      ofp;
    seqdec_t*  seqdec;
    qualdec_t* qualdec;
    auxdec_t*  auxdec;
} filedec_t;

filedec_t* filedec_new(FILE* ifp, FILE* ofp);
void filedec_free(filedec_t* filedec);
void filedec_decode(filedec_t* filedec);

#endif /*TSC_FILECODEC_H */
