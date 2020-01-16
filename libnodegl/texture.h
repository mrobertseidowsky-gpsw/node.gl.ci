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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "glincludes.h"
#include "utils.h"
#include <vulkan/vulkan.h>

struct ngl_ctx;

enum {
    NGLI_MIPMAP_FILTER_NONE,
    NGLI_MIPMAP_FILTER_NEAREST,
    NGLI_MIPMAP_FILTER_LINEAR,
    NGLI_NB_MIPMAP
};

enum {
    NGLI_FILTER_NEAREST,
    NGLI_FILTER_LINEAR,
    NGLI_NB_FILTER
};

#ifndef VULKAN_BACKEND
GLint ngli_texture_get_gl_min_filter(int min_filter, int mipmap_filter);
GLint ngli_texture_get_gl_mag_filter(int mag_filter);
#else
VkFilter ngli_texture_get_vk_filter(int filter);
VkFilter ngli_texture_get_vk_mipmap_mode(int mipmap_filter);
#endif

enum {
    NGLI_WRAP_CLAMP_TO_EDGE,
    NGLI_WRAP_MIRRORED_REPEAT,
    NGLI_WRAP_REPEAT,
    NGLI_NB_WRAP
};

#ifndef VULKAN_BACKEND
GLint ngli_texture_get_gl_wrap(int wrap);
#else
VkSamplerAddressMode ngli_texture_get_vk_wrap(int wrap);
#endif

enum {
    NGLI_ACCESS_UNDEFINED,
    NGLI_ACCESS_READ_BIT,
    NGLI_ACCESS_WRITE_BIT,
    NGLI_ACCESS_READ_WRITE,
    NGLI_ACCESS_NB
};

NGLI_STATIC_ASSERT(texture_access, (NGLI_ACCESS_READ_BIT | NGLI_ACCESS_WRITE_BIT) == NGLI_ACCESS_READ_WRITE);

#ifndef VULKAN_BACKEND
GLenum ngli_texture_get_gl_access(int access);
#endif

#define NGLI_TEXTURE_PARAM_DEFAULTS {          \
    .dimensions = 2,                           \
    .format = NGLI_FORMAT_UNDEFINED,           \
    .min_filter = NGLI_FILTER_NEAREST,         \
    .mag_filter = NGLI_FILTER_NEAREST,         \
    .mipmap_filter = NGLI_MIPMAP_FILTER_NONE,  \
    .wrap_s = NGLI_WRAP_CLAMP_TO_EDGE,         \
    .wrap_t = NGLI_WRAP_CLAMP_TO_EDGE,         \
    .wrap_r = NGLI_WRAP_CLAMP_TO_EDGE,         \
    .access = NGLI_ACCESS_READ_WRITE           \
}

#define NGLI_TEXTURE_USAGE_ATTACHMENT_ONLY (1 << 0)

struct texture_params {
    int dimensions;
    int format;
    int width;
    int height;
    int depth;
    int samples;
    int min_filter;
    int mag_filter;
    int mipmap_filter;
    int wrap_s;
    int wrap_t;
    int wrap_r;
    int access;
    int immutable;
    int usage;
    int external_storage;
    int external_oes;
    int rectangle;
    int cubemap;
};

struct texture {
    struct ngl_ctx *ctx;
    struct texture_params params;
    int wrapped;
    int external_storage;
    int bytes_per_pixel;

#ifdef VULKAN_BACKEND
    VkCommandPool command_pool;
    VkFormat format;

    VkBuffer buffer;
    VkDeviceMemory buffer_memory;
    VkImage image;
    VkImageLayout image_layout;
    VkDeviceSize image_size;
    int image_allocated;
    VkDeviceMemory image_memory;
    VkImageView image_view;
    VkSampler image_sampler;
#else
    GLenum target;
    GLuint id;
    GLint format;
    GLint internal_format;
    GLenum format_type;
#endif
};

int ngli_texture_init(struct texture *s,
                      struct ngl_ctx *ctx,
                      const struct texture_params *params);

#ifndef VULKAN_BACKEND
int ngli_texture_wrap(struct texture *s,
                      struct ngl_ctx *ctx,
                      const struct texture_params *params,
                      GLuint id);

void ngli_texture_set_id(struct texture *s, GLuint id);
void ngli_texture_set_dimensions(struct texture *s, int width, int height, int depth);
#endif

int ngli_texture_has_mipmap(const struct texture *s);
int ngli_texture_match_dimensions(const struct texture *s, int width, int height, int depth);

int ngli_texture_upload(struct texture *s, const uint8_t *data, int linesize);
int ngli_texture_generate_mipmap(struct texture *s);

void ngli_texture_reset(struct texture *s);

#endif
