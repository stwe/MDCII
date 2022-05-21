#version 330

layout (location = 0) in vec4 aPosition;

out vec2 vUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition.xy, 0.0, 1.0);
    vUv = aPosition.zw;
}
