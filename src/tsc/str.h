// Copyright 2015 Leibniz University Hannover (LUH)

#ifndef TSC_STR_H_
#define TSC_STR_H_

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct str_t_ {
    char *s;
    size_t len;
    size_t sz;
} str_t;

str_t *str_new();

void str_free(str_t *str);

void str_clear(str_t *str);

void str_reserve(str_t *str, size_t sz);

void str_extend(str_t *str, size_t ex);

void str_trunc(str_t *str, size_t tr);

void str_append_str(str_t *str, const str_t *app);

void str_append_cstr(str_t *str, const char *cstr);

void str_append_cstrn(str_t *str, const char *cstr, size_t len);

void str_append_char(str_t *str, char c);

void str_append_int(str_t *str, int64_t num);

void str_copy_str(str_t *dest, const str_t *src);

void str_copy_cstr(str_t *dest, const char *src);

#endif  // TSC_STR_H_
