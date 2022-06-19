#version 330

out vec4 fragColor;

in vec2 vUv;

uniform sampler2D diffuseMap;
uniform float selected;

void main()
{
    fragColor = texture(diffuseMap, vUv);

    if (selected > 0.5)
    {
        fragColor /= 2.0;
    }

    if (fragColor.a == 0)
    {
        discard;
    }
}
