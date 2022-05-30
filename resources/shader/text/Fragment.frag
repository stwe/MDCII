#version 330

in vec2 vUv;

out vec4 fragColor;

uniform sampler2D textTexture;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textTexture, vUv).r);
    fragColor = vec4(textColor, 1.0) * sampled;
}
