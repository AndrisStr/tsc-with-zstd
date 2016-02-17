/*
 * The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third
 * party and contributor rights, including patent rights, and no such
 * rights are granted under this license.
 *
 * Copyright (c) 2015-2016, Leibniz Universitaet Hannover, Institut fuer
 * Informationsverarbeitung (TNT)
 * Contact: <voges@tnt.uni-hannover.de>
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * Neither the name of the TNT nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE

#include "tsclib.h"
#include "osro.h"
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

static bool tsc_yesno(void)
{
    int c = getchar();
    bool yes = c == 'y' || c == 'Y';
    while (c != '\n' && c != EOF)
        c = getchar();
    return yes;
}

void tsc_cleanup(void)
{
    if (tsc_in_fp != NULL)
        osro_fclose(tsc_in_fp);
    if (tsc_out_fp != NULL)
        osro_fclose(tsc_out_fp);
    if (tsc_out_fname->len > 0) {
        tsc_log("Do you want to remove %s (y/n)? ", tsc_out_fname->s);
        if (tsc_yesno()) {
            unlink((const char *)tsc_out_fname->s);
            tsc_log("Removed %s\n", tsc_out_fname->s);
        }
    }
}

void tsc_abort(void)
{
    tsc_cleanup();
    exit(EXIT_FAILURE);
}

void tsc_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char *msg;
    vasprintf(&msg, fmt, args);
    va_end(args);
    fprintf(stdout, "%s", msg);
    free(msg);
}

void tsc_error(const char *fmt, ...)
{


    va_list args;
    va_start(args, fmt);
    char *msg;
    vasprintf(&msg, fmt, args);
    va_end(args);
    fprintf(stdout, "Error: %s", msg);
    free(msg);
    tsc_abort();
}

