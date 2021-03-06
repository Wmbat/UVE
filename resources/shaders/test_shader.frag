#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 frag_colour;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(frag_colour, 1.0);
}
