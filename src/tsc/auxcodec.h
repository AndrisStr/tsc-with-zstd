// Copyright 2015 Leibniz University Hannover (LUH)

#ifndef TSC_AUXCODEC_H_
#define TSC_AUXCODEC_H_

#include <stdint.h>
#include <stdio.h>

#include "str.h"

typedef struct auxcodec_t_ {
    size_t record_cnt;
    str_t *uncompressed;
    unsigned char *compressed;
} auxcodec_t;

auxcodec_t *auxcodec_new();

void auxcodec_free(auxcodec_t *auxcodec);

// Encoder methods
// -----------------------------------------------------------------------------

void auxcodec_add_record(auxcodec_t *auxcodec, uint16_t flag, uint8_t mapq, const char *opt);

size_t auxcodec_write_block(auxcodec_t *auxcodec, FILE *fp);

// Decoder methods
// -----------------------------------------------------------------------------

size_t auxcodec_decode_block(auxcodec_t *auxcodec, FILE *fp, uint16_t *flag, uint8_t *mapq, str_t **opt);

#endif  // TSC_AUXCODEC_H_
