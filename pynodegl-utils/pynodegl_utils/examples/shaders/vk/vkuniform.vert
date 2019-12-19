#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 ngl_position;

layout(push_constant) uniform ngl_block {
    mat4 modelview_matrix;
    mat4 projection_matrix;
} ngl;

layout(binding = 0) uniform VertexParameter {
    vec4  color0;
    vec4  color1;
    vec4  color2;
} parameters;

layout(location = 0) out vec4 color;

void main()
{
    gl_Position = ngl.projection_matrix * ngl.modelview_matrix * vec4(ngl_position, 1.0);
    color = parameters.color2;
}
