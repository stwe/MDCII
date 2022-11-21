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

uniform mat4 projectionView;
uniform int worldRotation;
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
// Animation
//-------------------------------------------------

int getFrame(int t_animCount)
{
    return updates % t_animCount;
}

void animateToolSmithy(int t_gfx, int t_rows)
{
    if (t_gfx >= 3984 && t_gfx <= 3999)
    {
        int animAdd = 16;
        int animAnz = 5;
        int frame = getFrame(animAnz);

        int gfxOffset = frame * animAdd;
        int newGfx = t_gfx + gfxOffset;

        uvOffset = calcUvOffset(newGfx, t_rows);
        vTextureAtlasIndex = calcTextureAtlasIndex(newGfx, t_rows);
    }
}

void animateFirestation(int t_gfx, int t_rows)
{
    if (t_gfx >= 2760 && t_gfx <= 2775)
    {
        int animAdd = 16;
        int animAnz = 6;
        int frame = getFrame(animAnz); // Werte von 0 bis 5

        int gfxOffset = frame * animAdd;
        int newGfx = t_gfx + gfxOffset;

        uvOffset = calcUvOffset(newGfx, t_rows);
        vTextureAtlasIndex = calcTextureAtlasIndex(newGfx, t_rows);
    }
}

void correctModelMatrix(int t_newHeight)
{
    mat4 m = modelMatrix[gl_InstanceID];

    /* scaling
        x 0 0 0
        0 y 0 0
        0 0 z 0
        0 0 0 1
    */
    m[1].y = t_newHeight;

    /* translation
        1 0 0 x
        0 1 0 y
        0 0 1 z
        0 0 0 1
    */
    m[3].y += (height - t_newHeight);

    gl_Position = projectionView * m * vec4(aPosition.xy, 0.0, 1.0);
    height = t_newHeight;
}

void animateWindmill(int t_gfx, int t_rows)
{
    if (t_gfx >= 1840 && t_gfx <= 1855)
    {
        int animAdd = 16;
        int animAnz = 16;
        int frame = getFrame(animAnz);

        int gfxOffset = frame * animAdd;
        int newGfx = t_gfx + gfxOffset;

        uvOffset = calcUvOffset(newGfx, t_rows);
        vTextureAtlasIndex = calcTextureAtlasIndex(newGfx, t_rows);
        correctModelMatrix(getHeight(newGfx));
    }
}

//-------------------------------------------------
// Main
//-------------------------------------------------

void main()
{
    gl_Position = projectionView * modelMatrix[gl_InstanceID] * vec4(aPosition.xy, 0.0, 1.0);

    int rows = int(nrOfRows);
    int gfx = int(gfxNumber[gl_InstanceID][worldRotation]);

    uvOffset = calcUvOffset(gfx, rows);
    vTextureAtlasIndex = calcTextureAtlasIndex(gfx, rows);
    height = getHeight(gfx);

    animateToolSmithy(gfx, rows);
    animateFirestation(gfx, rows);
    animateWindmill(gfx, rows);

    uv = aPosition.zw;

    vUv.x = (uv.x / nrOfRows) + uvOffset.x;
    vUv.y = (uv.y / nrOfRows) + uvOffset.y;

    if (uv.y == 1.0)
    {
        vUv.y = ((1.0 / nrOfRows) * height / maxY) + uvOffset.y;
    }
}
