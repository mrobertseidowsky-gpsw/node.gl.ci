#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform FragmentParameters {
    vec2 factor0;
    vec4 factor1;
    vec3 factor2;
} parameters;

layout(binding = 2) uniform VertexParameter {
    vec4  color0;
    vec4  color1;
    vec4  color2;
} parameters2;


layout(location = 0) in vec4 input_color;
layout(location = 0) out vec4 output_color;

void main() {
    output_color = vec4(1.0, 0.0, 0.0, 1.0);
}
