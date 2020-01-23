/*
 * Copyright 2019 GoPro Inc.
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

#include "default_shaders.h"
#include "program.h"

#ifdef VULKAN_BACKEND
static const char default_fragment_shader[] =
    "#version 450"                                                                      "\n"
    "#extension GL_ARB_separate_shader_objects : enable"                                "\n"
    ""                                                                                  "\n"
    "layout(location = 3) in vec3 fragColor;"                                           "\n"
    "layout(location = 0) out vec4 outColor;"                                           "\n"
    ""                                                                                  "\n"
    "void main()"                                                                       "\n"
    "{"                                                                                 "\n"
        "outColor = vec4(fragColor, 1.0);"                                              "\n"
    "}";

static const char default_vertex_shader[] =
    "#version 450"                                                                                  "\n"
    "#extension GL_ARB_separate_shader_objects : enable"                                            "\n"
    ""                                                                                              "\n"
    "//precision highp float;"                                                                      "\n"
    ""                                                                                              "\n"
    "out gl_PerVertex {"                                                                            "\n"
        "vec4 gl_Position;"                                                                         "\n"
    "};"                                                                                            "\n"
    ""                                                                                              "\n"
    "/* node.gl */"                                                                                 "\n"
    "layout(location = 0) in vec3 ngl_position;"                                                    "\n"
    "//layout(location = 1) in vec2 ngl_uvcoord;"                                                   "\n"
    "//layout(location = 2) in vec3 ngl_normal;"                                                    "\n"
    ""                                                                                              "\n"
    "layout(push_constant) uniform ngl_block {"                                                     "\n"
        "mat4 modelview_matrix;"                                                                    "\n"
        "mat4 projection_matrix;"                                                                   "\n"
        "//mat3 normal_matrix;"                                                                     "\n"
    "} ngl;"                                                                                        "\n"
    ""                                                                                              "\n"
    "//uniform mat4 ngl_modelview_matrix;"                                                          "\n"
    "//uniform mat4 ngl_projection_matrix;"                                                         "\n"
    "//uniform mat3 ngl_normal_matrix;"                                                             "\n"
    ""                                                                                              "\n"
    "//uniform mat4 tex0_coord_matrix;"                                                             "\n"
    "//uniform vec2 tex0_dimensions;"                                                               "\n"
    ""                                                                                              "\n"
    "//layout(location = 0) out vec2 var_uvcoord;"                                                  "\n"
    "//layout(location = 1) out vec3 var_normal;"                                                   "\n"
    "//layout(location = 2) out vec2 var_tex0_coord;"                                               "\n"
    ""                                                                                              "\n"
    "/* custom */"                                                                                  "\n"
    "//layout(location = 3) in vec3 color;"                                                         "\n"
    "//layout(location = 3) out vec3 fragColor;"                                                    "\n"
    ""                                                                                              "\n"
    "void main()"                                                                                   "\n"
    "{"                                                                                             "\n"
        "gl_Position = ngl.projection_matrix * ngl.modelview_matrix * vec4(ngl_position, 1.0);"     "\n"
        "//var_uvcoord = ngl_uvcoord;"                                                              "\n"
        "//var_normal = ngl.normal_matrix * ngl_normal;"                                            "\n"
        "//var_tex0_coord = (tex0_coord_matrix * vec4(ngl_uvcoord, 0, 1)).xy;"                      "\n"
        "//fragColor = color;"                                                                      "\n"
    "}";
#else
static const char default_vertex_shader[] =
    "#version 100"                                                                      "\n"
    ""                                                                                  "\n"
    "precision highp float;"                                                            "\n"
    "attribute vec4 ngl_position;"                                                      "\n"
    "attribute vec2 ngl_uvcoord;"                                                       "\n"
    "attribute vec3 ngl_normal;"                                                        "\n"
    "uniform mat4 ngl_modelview_matrix;"                                                "\n"
    "uniform mat4 ngl_projection_matrix;"                                               "\n"
    "uniform mat3 ngl_normal_matrix;"                                                   "\n"

    "uniform mat4 tex0_coord_matrix;"                                                   "\n"

    "varying vec2 var_uvcoord;"                                                         "\n"
    "varying vec3 var_normal;"                                                          "\n"
    "varying vec2 var_tex0_coord;"                                                      "\n"
    "void main()"                                                                       "\n"
    "{"                                                                                 "\n"
    "    gl_Position = ngl_projection_matrix * ngl_modelview_matrix * ngl_position;"    "\n"
    "    var_uvcoord = ngl_uvcoord;"                                                    "\n"
    "    var_normal = ngl_normal_matrix * ngl_normal;"                                  "\n"
    "    var_tex0_coord = (tex0_coord_matrix * vec4(ngl_uvcoord, 0.0, 1.0)).xy;"        "\n"
    "}";

#if defined(TARGET_ANDROID)
static const char default_fragment_shader[] =
    "#version 100"                                                                      "\n"
    "#extension GL_OES_EGL_image_external : require"                                    "\n"
    ""                                                                                  "\n"
    "precision highp float;"                                                            "\n"
    "uniform int tex0_sampling_mode;"                                                   "\n"
    "uniform sampler2D tex0_sampler;"                                                   "\n"
    "uniform samplerExternalOES tex0_external_sampler;"                                 "\n"
    "varying vec2 var_uvcoord;"                                                         "\n"
    "varying vec2 var_tex0_coord;"                                                      "\n"
    "void main(void)"                                                                   "\n"
    "{"                                                                                 "\n"
    "    if (tex0_sampling_mode == 1)"                                                  "\n"
    "        gl_FragColor = texture2D(tex0_sampler, var_tex0_coord);"                   "\n"
    "    else if (tex0_sampling_mode == 2)"                                             "\n"
    "        gl_FragColor = texture2D(tex0_external_sampler, var_tex0_coord);"          "\n"
    "    else"                                                                          "\n"
    "        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"                                  "\n"
    "}";
#else
static const char default_fragment_shader[] =
    "#version 100"                                                                      "\n"
    ""                                                                                  "\n"
    "precision highp float;"                                                            "\n"
    "uniform sampler2D tex0_sampler;"                                                   "\n"
    "varying vec2 var_uvcoord;"                                                         "\n"
    "varying vec2 var_tex0_coord;"                                                      "\n"
    "void main(void)"                                                                   "\n"
    "{"                                                                                 "\n"
    "    gl_FragColor = texture2D(tex0_sampler, var_tex0_coord);"                       "\n"
    "}";
#endif
#endif

static const char * const shader_map[NGLI_PROGRAM_SHADER_NB] = {
    [NGLI_PROGRAM_SHADER_VERT] = default_vertex_shader,
    [NGLI_PROGRAM_SHADER_FRAG] = default_fragment_shader,
};

const char *ngli_get_default_shader(int stage)
{
    return shader_map[stage];
}
