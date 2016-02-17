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

#ifndef CBUFSTR_H
#define CBUFSTR_H

#include "str.h"
#include <stdlib.h>

typedef struct cbufstr_t_ {
    size_t sz;    // size of circular buffer
    str_t  **buf; // array holding the strings in the buffer
    size_t nxt;   // next free position
    size_t n;     // number of elements currently in buffer
} cbufstr_t;

cbufstr_t *cbufstr_new(const size_t sz);
void cbufstr_free(cbufstr_t *cbufstr);
void cbufstr_clear(cbufstr_t *cbufstr);
void cbufstr_push(cbufstr_t *cbufstr, const char *s);
str_t * cbufstr_top(cbufstr_t *cbufstr);
str_t * cbufstr_get(const cbufstr_t *cbufstr, const size_t pos);

#endif // CBUFSTR_H

