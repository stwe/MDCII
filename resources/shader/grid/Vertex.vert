#version 430

layout (location = 0) in vec4 aPosition;

layout(std140, binding = 0) buffer modelMatrices
{
    mat4 modelMatrix[];
};

out vec2 vUv;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * modelMatrix[gl_InstanceID] * vec4(aPosition.xy, 0.0, 1.0);
    vUv = aPosition.zw;
}
