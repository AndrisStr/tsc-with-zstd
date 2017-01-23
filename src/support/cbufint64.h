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

#ifndef TSC_CBUFINT64_H
#define TSC_CBUFINT64_H

#include <stdint.h>
#include <stdlib.h>

typedef struct cbufint64_t_ {
    size_t   sz;  // size of circular buffer
    int64_t* buf; // array holding the integers in the buffer
    size_t   nxt; // next free position
    size_t   n;   // number of elements currently in buffer
} cbufint64_t;

cbufint64_t * cbufint64_new(const size_t sz);
void cbufint64_free(cbufint64_t *cbufint64);
void cbufint64_clear(cbufint64_t *cbufint64);
void cbufint64_push(cbufint64_t *cbufint64, const int64_t x);
int64_t cbufint64_top(cbufint64_t *cbufint64);
int64_t cbufint64_get(const cbufint64_t *cbufint64, const size_t pos);

#endif // TSC_CBUFINT64_H
