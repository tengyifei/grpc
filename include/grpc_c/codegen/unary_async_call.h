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

#ifndef GRPC_C_CODEGEN_UNARY_ASYNC_CALL_H
#define GRPC_C_CODEGEN_UNARY_ASYNC_CALL_H

#include <grpc_c/codegen/message.h>
#include <grpc_c/codegen/method.h>
#include <grpc_c/completion_queue.h>
#include <grpc_c/grpc_c.h>

GRPC_client_async_response_reader *GRPC_unary_async_call(
    GRPC_completion_queue *cq, const GRPC_method rpc_method,
    const GRPC_message request, GRPC_client_context *const context);

void GRPC_client_async_finish(GRPC_client_async_response_reader *reader,
                              void *response, void *tag);

void GRPC_client_async_read_metadata(GRPC_client_async_response_reader *reader,
                                     void *tag);

GRPC_server_async_response_writer *GRPC_unary_async_server_request(
    GRPC_registered_service *service, size_t method_index,
    GRPC_server_context *const context, void *request,
    GRPC_incoming_notification_queue *incoming_queue,
    GRPC_completion_queue *processing_queue, void *tag);

void GRPC_unary_async_server_finish(GRPC_server_async_response_writer *writer,
                                    const GRPC_message response,
                                    const grpc_status_code server_status,
                                    void *tag);

#endif /* GRPC_C_CODEGEN_UNARY_ASYNC_CALL_H */
