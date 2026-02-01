#version 330
#define MAX_LIGHTS 16

in vec3 fragNormal;
in vec3 fragWorldPos;

uniform vec4 baseColor;

uniform int lightCount;
uniform float emissiveIntensity;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform float lightIntensity[MAX_LIGHTS];

out vec4 finalColor;

void main()
{
    vec3 n = normalize(fragNormal);
    vec3 color = vec3(0.0);
    color += baseColor.rgb * 0.06;

    for (int i = 0; i < lightCount; i++) {
        vec3 L = lightPos[i] - fragWorldPos;
        float dist = length(L);
        vec3 l = L / dist;

        float ndotl = dot(n, l);

        float wrap = 0.2;
        float diffuse = clamp((ndotl + wrap) / (1.0 + wrap), 0.0, 1.0);
        diffuse = pow(diffuse, 1.2);

        float rim = smoothstep(0.4, 1.0, 1.0 - max(dot(n, -l), 0.0)) * 0.25;

        float attenuation = lightIntensity[i] / (dist * dist + 25.0);
        attenuation = clamp(attenuation, 0.0, 1.0);

        float lightEnergy = clamp(diffuse + rim, 0.0, 1.0) * attenuation * 0.8;

        vec3 tint = normalize(lightColor[i] + 1e-5);

        vec3 lit = baseColor.rgb * lightEnergy;
        lit *= mix(vec3(1.0), tint, 0.35);

        color += lit;
    }

    color += baseColor.rgb * emissiveIntensity * 0.5;
    finalColor = vec4(color, baseColor.a);
}
