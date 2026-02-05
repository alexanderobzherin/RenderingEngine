#version 450

// -------------
// Inputs from vertex shader
// -------------

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragFontColor;

layout(std140, binding = 1) uniform PackedParametersUBO
{
    vec4 fontColor;
    vec4 OutlineColor;
    float OutlineThicknessPx; 
    float InvAtlasSizeWidth;
    float InvAtlasSizeHeight;  
} packedParamsUbo;

layout(binding = 2) uniform sampler2D texSampler;

// -------------
// Outputs
// -------------

layout(location = 0) out vec4 outColor;


void main()
{
    if(packedParamsUbo.OutlineThicknessPx == 0.0)
    {
        float coverage = texture(texSampler, fragUV).r;
        vec4 color = fragFontColor;
        outColor = vec4(color.rgb * coverage, color.a * coverage);
        return;
    }
    
    float c = texture(texSampler, fragUV).r;

    if (c > 0.0)
    {
        outColor = vec4(fragFontColor.rgb * c, fragFontColor.a * c);
        return;
    }

    const float invX = packedParamsUbo.InvAtlasSizeWidth;
    const float invY = packedParamsUbo.InvAtlasSizeHeight;
    float n = 0.0;

    for(float round = 1; round <= packedParamsUbo.OutlineThicknessPx; round += 1.0)
    {
        n = max(n, texture(texSampler, fragUV + vec2(invX * round, 0.0)).r); // Left side
        n = max(n, texture(texSampler, fragUV + vec2(-invX * round, 0.0)).r); // Right side
        n = max(n, texture(texSampler, fragUV + vec2(0.0,  invY * round)).r); // Top side
        n = max(n, texture(texSampler, fragUV + vec2(0.0, -invY * round)).r); // Bottom side
    }

    if (n > 0.0)
    {
        outColor = packedParamsUbo.OutlineColor;
    }
    else
    {
        outColor = vec4(0.0);
    }
}