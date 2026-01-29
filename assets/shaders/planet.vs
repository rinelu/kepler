#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 model;

out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
    vec4 worldPos = model * vec4(vertexPosition, 1.0);
    fragWorldPos = worldPos.xyz;
    fragNormal = normalize(mat3(model) * vertexNormal);
    // gragNormal = normalize(mat3(transpose(inverse(model))) * vertexNormal);

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
