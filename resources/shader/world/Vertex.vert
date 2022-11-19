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
    int textureHeight[];
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
uniform int updates;

//-------------------------------------------------
// Globals
//-------------------------------------------------

vec2 uvOffset;
vec2 uv;
int height;

//-------------------------------------------------
// Helper
//-------------------------------------------------

vec2 calcUvOffset(int t_gfx, int t_rows)
{
    return vec2(
        (t_gfx % t_rows) / nrOfRows,
        (t_gfx / t_rows) / nrOfRows
    );
}

int calcTextureAtlasIndex(int t_gfx, int t_rows)
{
    const int NO_GFX = -1;
    if (t_gfx == NO_GFX)
    {
        return NO_GFX;
    }

    return (t_gfx / (t_rows * t_rows));
}

int getHeight(int t_gfx)
{
    return int(textureHeight[t_gfx]);
}

//-------------------------------------------------
// Test Windmill Animation
//-------------------------------------------------

void animateWindmill(int t_gfx, int t_rows)
{
    if (t_gfx >= 1840 && t_gfx <= 1840 + 256)
    {
        int startOffset = 1840 + 16;
        int diff = t_gfx - startOffset;
        int rest = diff % 16;
        int start = 1840 + rest;

        int gfxOffset = updates;
        gfxOffset *= 16;
        int gfx = start + gfxOffset;

        uvOffset = calcUvOffset(gfx, t_rows);
        vTextureAtlasIndex = calcTextureAtlasIndex(gfx, t_rows);
    }
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
    height = getHeight(gfx);

    // hardcoded for testing
    animateWindmill(gfx, rows);

    uv = aPosition.zw;

    vUv.x = (uv.x / nrOfRows) + uvOffset.x;
    vUv.y = (uv.y / nrOfRows) + uvOffset.y;

    if (uv.y == 1.0)
    {
        float h = float(height);
        vUv.y = ((1.0 / nrOfRows) * h / maxY) + uvOffset.y;
    }
}
