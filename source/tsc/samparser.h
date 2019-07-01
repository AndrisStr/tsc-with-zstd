#ifndef TSC_SAMPARSER_H
#define TSC_SAMPARSER_H

#include "samrec.h"
#include "str.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct samparser_t_ {
    FILE     *fp;   // file pointer
    str_t    *head; // SAM header
    samrec_t curr;  // current SAM record
} samparser_t;

samparser_t * samparser_new(FILE *fp);
void samparser_free(samparser_t *samparser);
void samparser_head(samparser_t *samparser);
bool samparser_next(samparser_t *samparser);

#endif // TSC_SAMPARSER_H