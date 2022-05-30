#version 330

layout (location = 0) in vec4 aVertex; // <vec2 pos, vec2 tex>

out vec2 vUv;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aVertex.xy, 0.0, 1.0);
    vUv = aVertex.zw;
}
