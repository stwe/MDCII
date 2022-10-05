#version 430

layout (location = 0) in vec4 aPosition;
layout (location = 1) in ivec4 aTextureAtlasIndex;
layout (location = 2) in vec4 aHeight;

layout(std140, binding = 0) buffer modelMatrices
{
    mat4 modelMatrix[];
};

layout(std430, binding = 1) buffer offsets
{
    vec2 offset[];
};

out vec2 vUv;
flat out int vTextureAtlasIndex;

uniform mat4 view;
uniform mat4 projection;
uniform int rotation;
uniform float maxY;
uniform float nrOfRows;

vec2 uvOffset;
vec2 uv;
float height;

void main()
{
    gl_Position = projection * view * modelMatrix[gl_InstanceID] * vec4(aPosition.xy, 0.0, 1.0);
    uvOffset = offset[gl_InstanceID];

    if (rotation == 0)
    {
        vTextureAtlasIndex = aTextureAtlasIndex.x;
        height = aHeight.x;
    }

    if (rotation == 1)
    {
        vTextureAtlasIndex = aTextureAtlasIndex.y;
        height = aHeight.y;
    }

    if (rotation == 2)
    {
        vTextureAtlasIndex = aTextureAtlasIndex.z;
        height = aHeight.z;
    }

    if (rotation == 3)
    {
        vTextureAtlasIndex = aTextureAtlasIndex.w;
        height = aHeight.w;
    }

    uv = aPosition.zw;

    vUv.x = (uv.x / nrOfRows) + uvOffset.x;
    vUv.y = (uv.y / nrOfRows) + uvOffset.y;

    if (uv.y == 1.0)
    {
        vUv.y = ((1.0 / nrOfRows) * height / maxY) + uvOffset.y;
    }
}
