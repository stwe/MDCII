#version 330

layout (location = 0) in vec4 aPosition;
layout (location = 1) in mat4 aModelMatrix0;
layout (location = 5) in mat4 aModelMatrix90;
layout (location = 9) in mat4 aModelMatrix180;
layout (location = 13) in mat4 aModelMatrix270;
layout (location = 17) in ivec4 aTextureAtlasIndex;
layout (location = 18) in vec2 aOffset0;
layout (location = 19) in vec2 aOffset90;
layout (location = 20) in vec2 aOffset180;
layout (location = 21) in vec2 aOffset270;
layout (location = 22) in vec4 aHeight;

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
    if (rotation == 0)
    {
        gl_Position = projection * view * aModelMatrix0 * vec4(aPosition.xy, 0.0, 1.0);
        uvOffset = aOffset0;
        vTextureAtlasIndex = aTextureAtlasIndex.x;
        height = aHeight.x;
    }

    if (rotation == 1)
    {
        gl_Position = projection * view * aModelMatrix90 * vec4(aPosition.xy, 0.0, 1.0);
        uvOffset = aOffset90;
        vTextureAtlasIndex = aTextureAtlasIndex.y;
        height = aHeight.y;
    }

    if (rotation == 2)
    {
        gl_Position = projection * view * aModelMatrix180 * vec4(aPosition.xy, 0.0, 1.0);
        uvOffset = aOffset180;
        vTextureAtlasIndex = aTextureAtlasIndex.z;
        height = aHeight.z;
    }

    if (rotation == 3)
    {
        gl_Position = projection * view * aModelMatrix270 * vec4(aPosition.xy, 0.0, 1.0);
        uvOffset = aOffset270;
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
