/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2015, Leibniz Universitaet Hannover, Institut fuer
 * Informationsverarbeitung (TNT)
 * Contact: <voges@tnt.uni-hannover.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the TNT nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TSC_TSCLIB_H
#define TSC_TSCLIB_H

#include "tvclib/str.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Safe debug macro
#if DBG
    #define DEBUG(c,...)\
        do {\
            fprintf(stderr, "Debug: %s:%d: %s: "c"\n", __FILE__, __LINE__, \
                    __FUNCTION__, ##__VA_ARGS__);\
        } while (false)
#else
    #define DEBUG(c,...) do { } while (false)
#endif

// Decimal prefixes
#define KB 1000LL
#define MB (KB*1000LL)
#define GB (MB*1000LL)

typedef enum {
    TSC_MODE_COMPRESS,
    TSC_MODE_DECOMPRESS,
    TSC_MODE_INFO
} tsc_mode_t;

typedef enum {
    TSC_LOG_DEFAULT,
    TSC_LOG_INFO,
    TSC_LOG_VERBOSE,
    TSC_LOG_WARN
} tsc_loglvl_t;

extern str_t *tsc_prog_name;
extern str_t *tsc_version;
extern str_t *tsc_in_fname;
extern str_t *tsc_out_fname;
extern FILE *tsc_in_fp;
extern FILE *tsc_out_fp;
extern tsc_mode_t tsc_mode;
extern tsc_loglvl_t tsc_loglvl;
extern bool tsc_stats;
extern unsigned int tsc_blocksz;

void tsc_cleanup(void);
void tsc_abort(void);
void tsc_error(const char *fmt, ...);
void tsc_log(tsc_loglvl_t loglvl, const char *fmt, ...);
void * tsc_malloc(const size_t n);
void * tsc_realloc(void *ptr, const size_t n);
FILE * tsc_fopen(const char *fname, const char *mode);
void tsc_fclose(FILE *fp);

#endif // TSC_TSCLIB_H

