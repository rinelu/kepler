#version 330

in vec3 vertexPosition;

uniform mat4 mvp;
uniform mat4 model;

out vec3 worldPos;

void main()
{
    vec4 wp = model * vec4(vertexPosition, 1.0);
    worldPos = wp.xyz;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
