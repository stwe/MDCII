#version 430

layout (location = 0) in vec4 aPosition;

layout(std140, binding = 0) buffer modelMatrices
{
    mat4 modelMatrix[];
};

layout(std430, binding = 1) buffer offsets
{
    vec2 offset[];
};

layout(std430, binding = 2) buffer textureAtlasIndices
{
    ivec4 textureAtlasIndex[];
};

layout(std430, binding = 3) buffer heights
{
    vec4 textureHeight[];
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
    vTextureAtlasIndex = int(textureAtlasIndex[gl_InstanceID][rotation]);
    height = float(textureHeight[gl_InstanceID][rotation]);

    uv = aPosition.zw;

    vUv.x = (uv.x / nrOfRows) + uvOffset.x;
    vUv.y = (uv.y / nrOfRows) + uvOffset.y;

    if (uv.y == 1.0)
    {
        vUv.y = ((1.0 / nrOfRows) * height / maxY) + uvOffset.y;
    }
}
