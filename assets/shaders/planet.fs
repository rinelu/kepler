#version 330

in vec3 fragNormal;

uniform vec4 baseColor;
uniform vec3 lightDir;

out vec4 finalColor;

float softLambert(vec3 n, vec3 l)
{
    float d = dot(n, l);
    return smoothstep(-0.2, 0.25, d);
}

void main()
{
    vec3 n = normalize(fragNormal);
    vec3 l = normalize(lightDir);

    float light = softLambert(n, l);
    vec3 color = baseColor.rgb * light;

    finalColor = vec4(color, baseColor.a);
}
