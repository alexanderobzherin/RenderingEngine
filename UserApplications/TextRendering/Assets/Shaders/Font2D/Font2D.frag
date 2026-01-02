#version 450

// -------------
// Inputs from vertex shader
// -------------

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragFontColor;

// -------------
// Outputs
// -------------

layout(location = 0) out vec4 outColor;

// -------------
// Optional texture sampler (uncomment if using textures)
// -------------
layout(binding = 2) uniform sampler2D texSampler;

void main()
{
    float coverage = texture(texSampler, fragUV).r;
    vec4 color = fragFontColor;
    outColor = vec4(color.rgb * coverage, color.a * coverage);
}