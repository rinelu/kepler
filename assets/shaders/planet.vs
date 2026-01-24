#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 model;

out vec3 fragNormal;

void main()
{
    fragNormal = mat3(transpose(inverse(model))) * vertexNormal;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
