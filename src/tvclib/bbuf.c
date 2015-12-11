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

#include "bbuf.h"
#include <stdio.h>
#include <string.h>

static void bbuf_init(bbuf_t *bbuf)
{
    bbuf->bytes = NULL;
    bbuf->sz = 0;
}

bbuf_t * bbuf_new(void)
{
    bbuf_t *bbuf = (bbuf_t *)malloc(sizeof(bbuf_t));
    if (!bbuf) abort();
    bbuf_init(bbuf);
    return bbuf;
}

void bbuf_free(bbuf_t *bbuf)
{
    if (bbuf != NULL) {
        if (bbuf->bytes != NULL) {
            free(bbuf->bytes);
            bbuf->bytes = NULL;
        }
        free(bbuf);
        bbuf = NULL;
    } else {
        fprintf(stderr, "Error: Tried to free null pointer\n");
        exit(EXIT_FAILURE);
    }
}

void bbuf_clear(bbuf_t *bbuf)
{
    if (bbuf->bytes != NULL) {
        free(bbuf->bytes);
        bbuf->bytes = NULL;
    }
    bbuf->sz = 0;
}

void bbuf_reserve(bbuf_t *bbuf, const size_t sz)
{
    bbuf->sz = sz;
    bbuf->bytes = (unsigned char *)realloc(bbuf->bytes, bbuf->sz);
    if (!bbuf->bytes) abort();
}

void bbuf_extend(bbuf_t *bbuf, const size_t ex)
{
    bbuf_reserve(bbuf, bbuf->sz + ex);
}

void bbuf_trunc(bbuf_t *bbuf, const size_t tr)
{
    bbuf->sz -= tr;
    bbuf_reserve(bbuf, bbuf->sz);
}

void bbuf_append_bbuf(bbuf_t *bbuf, const bbuf_t *app)
{
    bbuf_extend(bbuf, app->sz);
    memcpy(bbuf->bytes + bbuf->sz, app->bytes, app->sz);
    bbuf->sz += app->sz;
}

void bbuf_append_byte(bbuf_t *bbuf, const char byte)
{
    bbuf_extend(bbuf, 1);
    bbuf->bytes[bbuf->sz++] = byte;
}

void bbuf_append_uint64(bbuf_t *bbuf, const uint64_t x)
{
    bbuf_append_byte(bbuf, (x >> 56) & 0xFF);
    bbuf_append_byte(bbuf, (x >> 48) & 0xFF);
    bbuf_append_byte(bbuf, (x >> 40) & 0xFF);
    bbuf_append_byte(bbuf, (x >> 32) & 0xFF);
    bbuf_append_byte(bbuf, (x >> 24) & 0xFF);
    bbuf_append_byte(bbuf, (x >> 16) & 0xFF);
    bbuf_append_byte(bbuf, (x >>  8) & 0xFF);
    bbuf_append_byte(bbuf, (x      ) & 0xFF);
}

void bbuf_append_buf(bbuf_t *bbuf, const unsigned char *buf, const size_t n)
{
    bbuf_extend(bbuf, n);
    memcpy(bbuf->bytes + bbuf->sz, buf, n);
    bbuf->sz += n;
}

