// Copyright 2015 Leibniz University Hannover (LUH)

#ifndef TSC_QUALCODEC_H_
#define TSC_QUALCODEC_H_

#include <stdio.h>

#include "str.h"

typedef struct qualcodec_t_ {
    size_t record_cnt;
    str_t *uncompressed;
    unsigned char *compressed;
} qualcodec_t;

qualcodec_t *qualcodec_new();

void qualcodec_free(qualcodec_t *qualcodec);

// Encoder methods
// -----------------------------------------------------------------------------

void qualcodec_add_record(qualcodec_t *qualcodec, const char *qual);

size_t qualcodec_write_block(qualcodec_t *qualcodec, FILE *fp);

// Decoder methods
// -----------------------------------------------------------------------------

size_t qualcodec_decode_block(qualcodec_t *qualcodec, FILE *fp, str_t **qual);

#endif  // TSC_QUALCODEC_H_
