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

#ifndef GPU_BENCH_H
#define GPU_BENCH_H

#include <stdint.h>

struct ngl_ctx;
struct gpu_bench;

struct gpu_bench *ngli_gpu_bench_create(void);
int ngli_gpu_bench_init(struct gpu_bench *s, struct ngl_ctx *ctx);
int ngli_gpu_bench_start(struct gpu_bench *s);
int ngli_gpu_bench_stop(struct gpu_bench *s);
int64_t ngli_gpu_bench_read(struct gpu_bench *s);
void ngli_gpu_bench_freep(struct gpu_bench **sp);

#endif
