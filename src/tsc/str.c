// Copyright 2015 Leibniz University Hannover (LUH)

#include "str.h"

#include <stdio.h>
#include <string.h>

#include "log.h"
#include "mem.h"

static void str_init(str_t *str) {
    str->s = NULL;
    str->len = 0;
    str->sz = 0;

    // Init string with terminating null byte
    str_reserve(str, 1);
    str->s[str->len] = '\0';
}

str_t *str_new() {
    str_t *str = (str_t *)tsc_malloc(sizeof(str_t));
    str_init(str);
    return str;
}

void str_free(str_t *str) {
    if (str != NULL) {
        if (str->s != NULL) {
            tsc_free(str->s);
            str->s = NULL;
        }
        tsc_free(str);
    } else {
        tsc_error("Tried to free null pointer\n");
    }
}

void str_clear(str_t *str) {
    if (str->s != NULL) {
        tsc_free(str->s);
        str->s = NULL;
    }
    str_init(str);
}

void str_reserve(str_t *str, const size_t sz) {
    str->sz = sz;
    str->s = (char *)tsc_realloc(str->s, str->sz * sizeof(char));
}

void str_extend(str_t *str, const size_t ex) { str_reserve(str, str->sz + ex); }

void str_trunc(str_t *str, const size_t tr) {
    str->len -= tr;
    str->sz -= tr;
    str_reserve(str, str->sz);
    str->s[str->len] = '\0';
}

void str_append_str(str_t *str, const str_t *app) {
    str_extend(str, app->len);
    memcpy(str->s + str->len, app->s, app->len);
    str->len += app->len;
    str->s[str->len] = '\0';
}

void str_append_cstr(str_t *str, const char *cstr) {
    size_t len = strlen(cstr);
    str_extend(str, len);
    memcpy(str->s + str->len, cstr, len);
    str->len += len;
    str->s[str->len] = '\0';
}

void str_append_cstrn(str_t *str, const char *cstr, const size_t len) {
    if (len > strlen(cstr)) {
        tsc_error("Failed to append C-string\n");
    }
    str_extend(str, len);
    memcpy(str->s + str->len, cstr, len);
    str->len += len;
    str->s[str->len] = '\0';
}

void str_append_char(str_t *str, const char c) {
    str_extend(str, 1);
    str->s[str->len++] = c;
    str->s[str->len] = '\0';
}

void str_append_int(str_t *str, const int64_t num) {
    char num_cstr[101];
    snprintf(num_cstr, sizeof(num_cstr), "%" PRId64, num);
    str_append_cstr(str, num_cstr);
}

void str_copy_str(str_t *dest, const str_t *src) {
    str_clear(dest);
    str_reserve(dest, src->len + 1);
    memcpy(dest->s, src->s, src->len);
    dest->len = src->len;
    dest->s[dest->len] = '\0';
}

void str_copy_cstr(str_t *str, const char *cstr) {
    str_clear(str);
    size_t len = strlen(cstr);
    str_reserve(str, len + 1);
    memcpy(str->s, cstr, len);
    str->len = len;
    str->s[str->len] = '\0';
}
