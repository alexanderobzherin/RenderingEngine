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
    vec2 glyphSize;
    vec2 glyphOffset;
    vec4 uvRect; // (u0, v0, u1, v1)
} packedParamsUbo;

// -------------
// Vertex inputs
// -------------

layout(location = 0) in vec2 inPos;      // [-0.5 .. +0.5]
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord; // unused now, but keep for compatibility

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
    // Convert quad from [-0.5..0.5] to [0..1]
    vec2 quad01 = inPos + vec2(0.5);

    // Scale quad to glyph size
    vec2 glyphPos = quad01 * packedParamsUbo.glyphSize;

    // Apply bearing offset (baseline alignment)
    glyphPos += packedParamsUbo.glyphOffset;

    // Final world position
    vec4 worldPos = vec4(glyphPos, 0.0, 1.0);

    gl_Position = ubo.proj * ubo.view * ubo.model * worldPos;

    // UV remapping into atlas
    fragUV = mix(
        packedParamsUbo.uvRect.xy,
        packedParamsUbo.uvRect.zw,
        quad01
    );

    fragFontColor = packedParamsUbo.fontColor;
}