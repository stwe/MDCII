#version 430

out vec4 fragColor;

in vec2 vUv;
flat in int vTextureAtlasIndex;

uniform sampler2DArray diffuseMap;
uniform float selected;

const int NO_TEXTURE_ATLAS = -1;

void main()
{
    if (vTextureAtlasIndex == NO_TEXTURE_ATLAS)
    {
        discard;
    }

    vec3 uv = vec3(vUv, vTextureAtlasIndex);
    fragColor = texture(diffuseMap, uv);

    if (selected > 0.5)
    {
        fragColor.r *= 2;
        fragColor.g *= 2;

        fragColor.r = clamp(fragColor.r, 0.0, 1.0);
        fragColor.g = clamp(fragColor.g, 0.0, 1.0);
    }

    if (fragColor.a == 0)
    {
        discard;
    }
}
