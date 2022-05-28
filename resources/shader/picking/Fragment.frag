#version 330

out vec4 fragColor;

in vec3 vIdColor;

void main()
{
    fragColor = vec4(vIdColor, 1.0);
}
