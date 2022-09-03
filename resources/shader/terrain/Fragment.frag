#version 330

out vec4 fragColor;

in vec2 vUv;
flat in int vTextureAtlasIndex;

uniform sampler2DArray sampler;
uniform float selected;

void main()
{
    vec3 uv = vec3(vUv, vTextureAtlasIndex);
    fragColor = texture(sampler, uv);

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
