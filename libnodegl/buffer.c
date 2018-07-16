/*
 * Copyright 2018 GoPro Inc.
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "buffer.h"
#include "glincludes.h"
#include "nodes.h"

int ngli_graphic_buffer_allocate(struct glcontext *gl,
                                 struct graphic_buffer *buffer,
                                 int size,
                                 int usage)
{
    if (!buffer->id) {
        ngli_glGenBuffers(gl, 1, &buffer->id);
        ngli_glBindBuffer(gl, GL_ARRAY_BUFFER, buffer->id);
        ngli_glBufferData(gl, GL_ARRAY_BUFFER, size, NULL, usage);
        buffer->size = size;
        buffer->usage = usage;
    }
    buffer->refcount++;
    return 0;
}

void ngli_graphic_buffer_bind(struct glcontext *gl,
                              struct graphic_buffer *buffer,
                              struct program *program,
                              int offset,
                              int size,
                              int index,
                              int type)
{
    ngli_glBindBufferRange(gl, type, index, buffer->id, offset, size);
}

void ngli_graphic_buffer_upload(struct glcontext *gl,
                                struct graphic_buffer *buffer,
                                void *data,
                                int size)
{
    ngli_glBindBuffer(gl, GL_ARRAY_BUFFER, buffer->id);
    ngli_glBufferSubData(gl, GL_ARRAY_BUFFER, 0, size, data);
}

void ngli_graphic_buffer_free(struct glcontext *gl,
                              struct graphic_buffer *buffer)
{
    if (!buffer)
        return;

    if (buffer->refcount-- == 1) {
        ngli_glDeleteBuffers(gl, 1, &buffer->id);
        buffer->id = 0;
    }
    ngli_assert(buffer->refcount >= 0);
}