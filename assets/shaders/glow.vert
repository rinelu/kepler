#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 model;

out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
    fragWorldPos = (model * vec4(vertexPosition, 1.0)).xyz;
    fragNormal   = normalize(mat3(model) * vertexNormal);
    gl_Position  = mvp * vec4(vertexPosition, 1.0);
}
