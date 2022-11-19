#version 430

//-------------------------------------------------
// In
//-------------------------------------------------

layout (location = 0) in vec4 aPosition;

layout(std140, binding = 0) buffer modelMatrices
{
    mat4 modelMatrix[];
};

layout(std430, binding = 1) buffer heights
{
    vec4 textureHeight[];
};

layout(std430, binding = 2) buffer gfxNumbers
{
    ivec4 gfxNumber[];
};

//-------------------------------------------------
// Out
//-------------------------------------------------

out vec2 vUv;
flat out int vTextureAtlasIndex;

//-------------------------------------------------
// Uniforms
//-------------------------------------------------

uniform mat4 view;
uniform mat4 projection;
uniform int rotation;
uniform float maxY;
uniform float nrOfRows;

//-------------------------------------------------
// Globals
//-------------------------------------------------

vec2 uvOffset;
vec2 uv;
float height;

//-------------------------------------------------
// Helper
//-------------------------------------------------

vec2 calcUvOffset(int gfx, int rows)
{
    return vec2(
        (gfx % rows) / nrOfRows,
        (gfx / rows) / nrOfRows
    );
}

int calcTextureAtlasIndex(int gfx, int rows)
{
    const int NO_GFX = -1;
    if (gfx == NO_GFX)
    {
        return NO_GFX;
    }

    return (gfx / (rows * rows));
}

//-------------------------------------------------
// Main
//-------------------------------------------------

void main()
{
    gl_Position = projection * view * modelMatrix[gl_InstanceID] * vec4(aPosition.xy, 0.0, 1.0);

    int rows = int(nrOfRows);
    int gfx = int(gfxNumber[gl_InstanceID][rotation]);
    uvOffset = calcUvOffset(gfx, rows);
    vTextureAtlasIndex = calcTextureAtlasIndex(gfx, rows);

    height = float(textureHeight[gl_InstanceID][rotation]);

    uv = aPosition.zw;

    vUv.x = (uv.x / nrOfRows) + uvOffset.x;
    vUv.y = (uv.y / nrOfRows) + uvOffset.y;

    if (uv.y == 1.0)
    {
        vUv.y = ((1.0 / nrOfRows) * height / maxY) + uvOffset.y;
    }
}
