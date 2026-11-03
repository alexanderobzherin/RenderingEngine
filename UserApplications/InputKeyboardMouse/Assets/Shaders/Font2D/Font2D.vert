#version 450

// -------------
// Uniform buffer
// -------------

layout(std140, binding = 0) uniform TransformationsUBO
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(std140, binding = 1) uniform PackedParametersUBO
{
    vec4 fontColor;
    vec4 OutlineColor;
    float OutlineThicknessPx;
    float InvAtlasSizeWidth;
    float InvAtlasSizeHeight;  
} packedParamsUbo;

// -------------
// Vertex inputs
// -------------

layout(location = 0) in vec2 inPos; 
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

// -------------
// Outputs to fragment shader
// -------------

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragFontColor;

// -------------
// Main
// -------------

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 0.0, 1.0);
    fragUV = inTexCoord;
    fragFontColor = packedParamsUbo.fontColor;
}