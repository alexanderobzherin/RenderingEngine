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
} packedParamsUbo;

// -------------
// Vertex inputs
// -------------

layout(location = 0) in vec2 inPos;      // vertex position
layout(location = 1) in vec4 inColor;    // vertex color
layout(location = 2) in vec2 inTexCoord; // vertex UV

// -------------
// Outputs to fragment shader
// -------------

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out vec4 fragFontColor;

// -------------
// Main
// -------------

void main()
{
    // Construct homogeneous 4D vertex position
    vec4 pos = vec4(inPos, 0.0, 1.0);

    // Apply model and view transforms
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 0.0, 1.0);

    // Pass interpolated data
    fragColor = inColor;
    fragUV = inTexCoord;
    fragFontColor = packedParamsUbo.fontColor;
}