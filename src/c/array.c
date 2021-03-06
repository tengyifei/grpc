/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "src/c/array.h"
#include <grpc/support/alloc.h>

void GRPC_array_init_impl(GRPC_array_state *state, void *data_ptr,
                          size_t elem_size) {
  void **data = data_ptr;
  state->capacity = 4;
  state->size = 0;
  *data = gpr_malloc(elem_size * 4);
}

void GRPC_array_pop_back_impl(GRPC_array_state *state, void *data_ptr,
                              size_t elem_size) {
  if (state->size == 0) return;
  void **data = data_ptr;
  state->size--;
  while (state->size * 2 + 1 <= state->capacity) {
    *data = gpr_realloc(*data, state->capacity / 2);
    state->capacity /= 2;
  }
}

void GRPC_array_ensure_capacity(GRPC_array_state *state, void *data_ptr,
                                size_t elem_size, size_t target_size) {
  if (target_size <= state->capacity) return;
  void **data = data_ptr;
  while (state->capacity < target_size) {
    *data = realloc(*data, state->capacity * 2);
    state->capacity *= 2;
  }
}

void GRPC_array_deinit_impl(GRPC_array_state *state, void *data_ptr,
                            size_t elem_size) {
  void **data = data_ptr;
  if (*data) {
    gpr_free(*data);
  }
}
