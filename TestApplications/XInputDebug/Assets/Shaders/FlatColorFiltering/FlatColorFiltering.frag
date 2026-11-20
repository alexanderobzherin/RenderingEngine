#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

layout(location = 2) in vec3 filteredColor;

const vec3 filteredColorRed = vec3(1.0f, 0.0f, 0.0f);
const vec3 BLACK = vec3(0.0f, 0.0f, 0.0f);

void main()
{
    vec3 textureColorRGB = vec3(texture(texSampler, fragTexCoord).rgb);
    vec3 resultColor = fragColor * textureColorRGB * filteredColorRed;

    if(resultColor == BLACK)
    {
        resultColor = filteredColor;
    }

    outColor = vec4(resultColor, 1.0);
}