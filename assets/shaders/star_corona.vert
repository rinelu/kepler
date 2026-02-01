#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 model;

out vec3 fragPosition;
out vec3 fragNormal;
out float shellRadius;

void main()
{
    vec4 worldPos = model * vec4(vertexPosition, 1.0);
    fragPosition = worldPos.xyz;
    fragNormal = normalize(mat3(model) * vertexNormal);
    shellRadius = length(vertexPosition);
    
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
