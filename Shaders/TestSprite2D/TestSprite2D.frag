#version 450

// -------------
// Inputs from vertex shader
// -------------

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

// -------------
// Outputs
// -------------

layout(location = 0) out vec4 outColor;

// -------------
// Optional texture sampler (uncomment if using textures)
// -------------
layout(binding = 1) uniform sampler2D texSampler;

void main()
{
    // If you later enable textures, replace this line with:
    outColor = texture(texSampler, fragUV) * fragColor;

    // outColor = fragColor;
}