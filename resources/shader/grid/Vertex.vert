#version 330

layout (location = 0) in vec4 aPosition;
layout (location = 1) in mat4 aModelMatrix0;
layout (location = 5) in mat4 aModelMatrix90;
layout (location = 9) in mat4 aModelMatrix180;
layout (location = 13) in mat4 aModelMatrix270;

out vec2 vUv;

uniform mat4 view;
uniform mat4 projection;
uniform int rotation;

void main()
{
    if (rotation == 0)
    {
        gl_Position = projection * view * aModelMatrix0 * vec4(aPosition.xy, 0.0, 1.0);
    }

    if (rotation == 1)
    {
        gl_Position = projection * view * aModelMatrix90 * vec4(aPosition.xy, 0.0, 1.0);
    }

    if (rotation == 2)
    {
        gl_Position = projection * view * aModelMatrix180 * vec4(aPosition.xy, 0.0, 1.0);
    }

    if (rotation == 3)
    {
        gl_Position = projection * view * aModelMatrix270 * vec4(aPosition.xy, 0.0, 1.0);
    }

    vUv = aPosition.zw;
}
