#version 430

//-------------------------------------------------
// Out
//-------------------------------------------------

out vec4 fragColor;

//-------------------------------------------------
// In
//-------------------------------------------------

in vec2 vUv;
flat in int vTextureAtlasIndex;
flat in int vSelected;

//-------------------------------------------------
// Uniforms
//-------------------------------------------------

uniform sampler2DArray diffuseMap;

//-------------------------------------------------
// Constants
//-------------------------------------------------

const int NO_TEXTURE_ATLAS = -1;

//-------------------------------------------------
// Main
//-------------------------------------------------

void main()
{
    if (vTextureAtlasIndex == NO_TEXTURE_ATLAS)
    {
        discard;
    }

    vec3 uv = vec3(vUv, vTextureAtlasIndex);
    fragColor = texture(diffuseMap, uv);

    if (vSelected == 1)
    {
        fragColor = fragColor * 0.5;
    }

    if (fragColor.a == 0)
    {
        discard;
    }
}
