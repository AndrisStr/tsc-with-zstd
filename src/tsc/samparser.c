// Copyright 2015 Leibniz University Hannover (LUH)

#include "samparser.h"

#include <string.h>

#include "log.h"
#include "mem.h"

static void samparser_init(samparser_t *samparser, FILE *fp) { samparser->fp = fp; }

samparser_t *samparser_new(FILE *fp) {
    samparser_t *samparser = (samparser_t *)tsc_malloc(sizeof(samparser_t));
    samparser->head = str_new();
    samparser_init(samparser, fp);
    return samparser;
}

void samparser_free(samparser_t *samparser) {
    if (samparser != NULL) {
        str_free(samparser->head);
        tsc_free(samparser);
    } else {
        tsc_error("Tried to free null pointer\n");
    }
}

void samparser_head(samparser_t *samparser) {
    // Read the SAM header
    bool sam_header = false;
    while (fgets(samparser->curr.line, sizeof(samparser->curr.line), samparser->fp)) {
        if (*(samparser->curr.line) == '@') {
            str_append_cstr(samparser->head, samparser->curr.line);
            sam_header = true;
        } else {
            if (!sam_header) tsc_error("SAM header missing\n");
            size_t offset = -strlen(samparser->curr.line);
            fseek(samparser->fp, (long)offset, SEEK_CUR);
            break;
        }
    }
}

static void samparser_parse(samparser_t *samparser) {
    size_t l = strlen(samparser->curr.line) - 1;

    while (l && (samparser->curr.line[l] == '\r' || samparser->curr.line[l] == '\n')) samparser->curr.line[l--] = '\0';

    char *c = samparser->curr.qname = samparser->curr.line;
    int f = 1;

    while (*c) {
        if (*c == '\t') {
            if (f == 1) samparser->curr.flag = (uint16_t)strtol(c + 1, NULL, 10);
            if (f == 2) samparser->curr.rname = c + 1;
            if (f == 3) samparser->curr.pos = (uint32_t)strtol(c + 1, NULL, 10);
            if (f == 4) samparser->curr.mapq = (uint8_t)strtol(c + 1, NULL, 10);
            if (f == 5) samparser->curr.cigar = c + 1;
            if (f == 6) samparser->curr.rnext = c + 1;
            if (f == 7) samparser->curr.pnext = (uint32_t)strtol(c + 1, NULL, 10);
            if (f == 8) samparser->curr.tlen = (int64_t)strtol(c + 1, NULL, 10);
            if (f == 9) samparser->curr.seq = c + 1;
            if (f == 10) samparser->curr.qual = c + 1;
            if (f == 11) samparser->curr.opt = c + 1;
            f++;
            *c = '\0';
            if (f == 12) break;
        }
        c++;
    }

    if (f == 11) samparser->curr.opt = c;
}

bool samparser_next(samparser_t *samparser) {
    // Try to read and parse next line
    if (fgets(samparser->curr.line, sizeof(samparser->curr.line), samparser->fp)) {
        if (*(samparser->curr.line) == '@')
            tsc_error("Tried to read SAM record but found header line\n");
        else
            samparser_parse(samparser);
    } else {
        return false;
    }
    return true;
}
