#ifndef TSC_SAMCODEC_H
#define TSC_SAMCODEC_H

#include "codecs/auxcodec.h"
#include "codecs/idcodec.h"
//#include "codecs/nuccodec_o0.h"
#include "codecs/nuccodec_o1.h"
#include "codecs/paircodec.h"
#include "codecs/qualcodec.h"
#include "samparser.h"
#include "tsclib/str.h"
#include <stdio.h>

typedef struct samcodec_t_ {
    FILE         *ifp;
    FILE         *ofp;
    unsigned int blk_sz;
    samparser_t  *samparser;
    auxcodec_t   *auxcodec;
    idcodec_t    *idcodec;
    nuccodec_t   *nuccodec;
    paircodec_t  *paircodec;
    qualcodec_t  *qualcodec;
} samcodec_t;

samcodec_t * samcodec_new(FILE *ifp, FILE *ofp, unsigned int blk_sz);
void samcodec_free(samcodec_t *samcodec);
void samcodec_encode(samcodec_t *samcodec);
void samcodec_decode(samcodec_t *samcodec);
void samcodec_info(samcodec_t *samcodec);

#endif // TSC_SAMCODEC_H
