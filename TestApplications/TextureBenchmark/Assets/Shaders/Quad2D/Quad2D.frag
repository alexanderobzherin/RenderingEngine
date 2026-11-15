#version 450

// -------------
// Inputs from vertex shader
// -------------

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;


layout(location = 0) out vec4 outColor;

layout(std140, binding = 1) uniform PackedParametersUBO
{
    vec4 color;
} packedParamsUbo;

layout(binding = 2) uniform sampler2D texSampler;

// -------------
// Outputs
// -------------

void main()
{
    outColor = texture(texSampler, fragUV) * fragColor;
}