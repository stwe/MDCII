#version 330

out vec4 fragColor;

in vec2 vUv;

uniform sampler2D diffuseMap;
uniform vec3 color;

void main()
{
    fragColor = texture(diffuseMap, vUv);
    if (fragColor.a == 0)
    {
        discard;
    }
    else
    {
        fragColor = vec4(color, 1.0);
    }
}
