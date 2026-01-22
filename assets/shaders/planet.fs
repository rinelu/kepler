#version 330

in vec3 fragNormal;

uniform vec4 baseColor;

out vec4 finalColor;

void main()
{
    vec3 n = normalize(fragNormal);
    float light = dot(n, normalize(vec3(-0.3, -1.0, -0.2))) * 0.5 + 0.5;
    finalColor = vec4(baseColor.rgb * light, baseColor.a);
}
