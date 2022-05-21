#version 330

out vec4 fragColor;

in vec2 vUv;

uniform sampler2D diffuseMap;

void main()
{
    fragColor = texture(diffuseMap, vUv);
    if (fragColor.a == 0)
    {
        discard;
    }

    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
