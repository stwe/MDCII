#version 430

out vec4 fragColor;

in vec2 vUv;

uniform sampler2D diffuseMap;
uniform float selected;

void main()
{
    fragColor = texture(diffuseMap, vUv);

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
