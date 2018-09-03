#ifndef TSC_QUALCODEC_H
#define TSC_QUALCODEC_H

#include "tsclib/str.h"
#include <stdio.h>

typedef struct qualcodec_t_ {
    size_t        record_cnt; // No. of records processed in the current block
    str_t         *uncompressed;
    unsigned char *compressed;
    size_t        compressed_sz;
} qualcodec_t;

qualcodec_t * qualcodec_new(void);
void qualcodec_free(qualcodec_t *qualcodec);

// Encoder methods
// -----------------------------------------------------------------------------

void qualcodec_add_record(qualcodec_t *qualcodec, const char *qual);
size_t qualcodec_write_block(qualcodec_t *qualcodec, FILE *fp);

// Decoder methods
// -----------------------------------------------------------------------------

size_t qualcodec_decode_block(qualcodec_t *qualcodec, FILE *fp, str_t **qual);

#endif // TSC_QUALCODEC_H
