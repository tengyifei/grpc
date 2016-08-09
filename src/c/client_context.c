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

#include "src/c/client_context.h"
#include <grpc/support/alloc.h>
#include <grpc_c/client_context.h>
#include <grpc_c/grpc_c.h>
#include "src/c/alloc.h"

grpc_client_context *GRPC_client_context_create(grpc_channel *chan) {
  grpc_client_context *context = GRPC_ALLOC_STRUCT(
      grpc_client_context,
      {.deadline = gpr_inf_future(GPR_CLOCK_REALTIME),
       .channel = chan,
       .serialization_impl = {.serialize = NULL, .deserialize = NULL},
       .status = {.ok = true}});
  return context;
}

void GRPC_client_context_destroy(GRPC_client_context **context) {
  if ((*context)->status.details) {
    gpr_free((*context)->status.details);
  }
  if ((*context)->call) {
    grpc_call_destroy((*context)->call);
    (*context)->call = NULL;
  }
  gpr_free(*context);
  *context = NULL;
}

GRPC_status GRPC_get_call_status(GRPC_client_context *context) {
  return context->status;
}

void GRPC_client_context_set_serialization_impl(
    GRPC_client_context *context, grpc_serialization_impl serialization_impl) {
  context->serialization_impl = serialization_impl;
}

// We define a conversion function instead of type-casting, which lets the user convert
// from any pointer to a grpc_context.
grpc_context *GRPC_client_context_to_base(grpc_client_context *client_context) {
  return (grpc_context *) client_context;
}
