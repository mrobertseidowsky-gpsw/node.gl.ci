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

#include <vulkan/vulkan.h>

#include "gpu_bench.h"
#include "memory.h"
#include "nodes.h"

struct gpu_bench {
    struct ngl_ctx *ctx;

    VkQueryPool query_pool;
};

struct gpu_bench *ngli_gpu_bench_create(void)
{
    struct gpu_bench *s = ngli_calloc(1, sizeof(*s));
    return s;
}

int ngli_gpu_bench_init(struct gpu_bench *s, struct ngl_ctx *ctx)
{
    s->ctx = ctx;

    struct glcontext *vk = ctx->glcontext;

    const VkQueryPoolCreateInfo query_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
        .queryType = VK_QUERY_TYPE_TIMESTAMP,
        .queryCount = 1,
    };
    VkResult vkret = vkCreateQueryPool(vk->device, &query_pool_create_info, NULL, &s->query_pool);
    if (vkret != VK_SUCCESS)
        return -1;

    return 0;
}

int ngli_gpu_bench_start(struct gpu_bench *s)
{
    //struct glcontext *vk = s->ctx->glcontext;
    //vkCmdWriteTimestamp();
    return 0;
}

int ngli_gpu_bench_stop(struct gpu_bench *s)
{
    //struct glcontext *vk = s->ctx->glcontext;
    //vkCmdWriteTimestamp();
    return 0;
}

int64_t ngli_gpu_bench_read(struct gpu_bench *s)
{
    //struct glcontext *vk = s->ctx->glcontext;
    return 0;
}

void ngli_gpu_bench_freep(struct gpu_bench **sp)
{
    struct gpu_bench *s = *sp;
    if (!s)
        return;
    struct glcontext *vk = s->ctx->glcontext;
    vkDestroyQueryPool(vk->device, s->query_pool, NULL);
    ngli_free(s);
    *sp = NULL;
}
