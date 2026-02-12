#version 450

// -------------
// Inputs from vertex shader
// -------------

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(std140, binding = 1) uniform PackedParametersUBO
{
    vec4 color;
} packedParamsUbo;

// -------------
// Outputs
// -------------

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = packedParamsUbo.color;
}