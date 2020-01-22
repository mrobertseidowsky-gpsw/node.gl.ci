/*
 * Copyright 2020 GoPro Inc.
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

#include "glincludes.h"
#include "gpu_bench.h"
#include "log.h"
#include "memory.h"
#include "nodes.h"

struct gpu_bench {
    struct ngl_ctx *ctx;

    int started;
    GLuint query;
    GLuint64 query_result;
    void (*glGenQueries)(const struct glcontext *gl, GLsizei n, GLuint * ids);
    void (*glDeleteQueries)(const struct glcontext *gl, GLsizei n, const GLuint * ids);
    void (*glBeginQuery)(const struct glcontext *gl, GLenum target, GLuint id);
    void (*glEndQuery)(const struct glcontext *gl, GLenum target);
    void (*glGetQueryObjectui64v)(const struct glcontext *gl, GLuint id, GLenum pname, GLuint64 *params);
};

static void noop(const struct glcontext *gl, ...)
{
}

struct gpu_bench *ngli_gpu_bench_create(void)
{
    struct gpu_bench *s = ngli_calloc(1, sizeof(*s));

    // We make sure these are available before init so that other gpu_bench API
    // call are always safe.
    s->glGenQueries          = (void *)noop;
    s->glDeleteQueries       = (void *)noop;
    s->glBeginQuery          = (void *)noop;
    s->glEndQuery            = (void *)noop;
    s->glGetQueryObjectui64v = (void *)noop;
    return s;
}

int ngli_gpu_bench_init(struct gpu_bench *s, struct ngl_ctx *ctx)
{
    struct glcontext *gl = ctx->glcontext;

    s->ctx = ctx;

    if (gl->features & NGLI_FEATURE_TIMER_QUERY) {
        s->glGenQueries          = ngli_glGenQueries;
        s->glDeleteQueries       = ngli_glDeleteQueries;
        s->glBeginQuery          = ngli_glBeginQuery;
        s->glEndQuery            = ngli_glEndQuery;
        s->glGetQueryObjectui64v = ngli_glGetQueryObjectui64v;
    } else if (gl->features & NGLI_FEATURE_EXT_DISJOINT_TIMER_QUERY) {
        s->glGenQueries          = ngli_glGenQueriesEXT;
        s->glDeleteQueries       = ngli_glDeleteQueriesEXT;
        s->glBeginQuery          = ngli_glBeginQueryEXT;
        s->glEndQuery            = ngli_glEndQueryEXT;
        s->glGetQueryObjectui64v = ngli_glGetQueryObjectui64vEXT;
    }

    s->glGenQueries(gl, 1, &s->query);
    return 0;
}

int ngli_gpu_bench_start(struct gpu_bench *s)
{
    struct ngl_ctx *ctx = s->ctx;
    struct glcontext *gl = ctx->glcontext;

    if (ctx->timer_active) {
        LOG(WARNING, "GPU timings will not be available when using multiple HUD "
            "in the same graph due to GL limitations");
    } else {
        // This specific instance of gpu_bench was able to grab the global
        // "timer active" lock
        s->started = 1;
        ctx->timer_active = 1;
        s->query_result = 0;
        s->glBeginQuery(gl, GL_TIME_ELAPSED, s->query);
    }
    return 0;
}

int ngli_gpu_bench_stop(struct gpu_bench *s)
{
    struct ngl_ctx *ctx = s->ctx;
    struct glcontext *gl = ctx->glcontext;

    if (s->started) {
        s->glEndQuery(gl, GL_TIME_ELAPSED);
        s->glGetQueryObjectui64v(gl, s->query, GL_QUERY_RESULT, &s->query_result);
        ctx->timer_active = 0;
        s->started = 0;
    }
    return 0;
}

int64_t ngli_gpu_bench_read(struct gpu_bench *s)
{
    return s->query_result;
}

void ngli_gpu_bench_freep(struct gpu_bench **sp)
{
    struct gpu_bench *s = *sp;
    if (!s)
        return;

    struct glcontext *gl = s->ctx->glcontext;
    s->glDeleteQueries(gl, 1, &s->query);
    ngli_free(s);
    *sp = NULL;
}
