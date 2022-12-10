#version 430

//-------------------------------------------------
// In
//-------------------------------------------------

layout (location = 0) in vec4 aPosition;

layout(std140, binding = 0) buffer modelMatrices
{
    mat4 modelMatrix[];
};

layout(std430, binding = 1) buffer gfxNumbers
{
    ivec4 gfxNumber[];
};

layout(std430, binding = 2) buffer buildingIds
{
    ivec4 buildingId[];
};

layout(std430, binding = 3) buffer heights
{
    float textureHeight[];
};

layout(std430, binding = 4) buffer buildingAnimations
{
    ivec4 buildingAnimation[];
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
uniform int updates[5];

//-------------------------------------------------
// Globals
//-------------------------------------------------

vec2 uvOffset;
vec2 uv;
float height;

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

float getHeight(int t_gfx)
{
    return textureHeight[t_gfx];
}

//-------------------------------------------------
// Animation
//-------------------------------------------------

int getFrame(int t_animCount, int t_animTime)
{
    switch(t_animTime)
    {
        case 90:
            return updates[0] % t_animCount;
        case 130:
            return updates[1] % t_animCount;
        case 150:
            return updates[2] % t_animCount;
        case 180:
            return updates[3] % t_animCount;
        case 220:
            return updates[4] % t_animCount;
    }
}

void correctModelMatrix(float t_newHeight)
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
    m[3].y += height - t_newHeight;

    gl_Position = projectionView * m * vec4(aPosition.xy, 0.0, 1.0);
    height = t_newHeight;
}

void animateBuilding(int t_gfx, int t_rows)
{
    int building = int(buildingId[gl_InstanceID][worldRotation]);
    ivec4 animation = ivec4(buildingAnimation[building]);

    int animAnz = animation.x;
    int animTime = animation.y;
    int animAdd = animation.w;

    // todo: bei -1 entspricht die Animation einer Gabäude-Variante (Gebäude steigt auf/ab)
    // todo: animAnz = Anzahl der Varianten
    if (animTime < 0)
    {
        return;
    }

    int frame = getFrame(animAnz, animTime);

    int gfxOffset = frame * animAdd;
    int newGfx = t_gfx + gfxOffset;

    uvOffset = calcUvOffset(newGfx, t_rows);
    vTextureAtlasIndex = calcTextureAtlasIndex(newGfx, t_rows);
    correctModelMatrix(getHeight(newGfx));
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

    animateBuilding(gfx, rows);

    uv = aPosition.zw;

    vUv.x = (uv.x / nrOfRows) + uvOffset.x;
    vUv.y = (uv.y / nrOfRows) + uvOffset.y;

    if (uv.y == 1.0)
    {
        vUv.y = ((1.0 / nrOfRows) * height / maxY) + uvOffset.y;
    }
}
