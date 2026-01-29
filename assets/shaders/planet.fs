#version 330
#define MAX_LIGHTS 4

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

    for (int i = 0; i < lightCount; i++) {
        vec3 L = lightPos[i] - fragWorldPos;
        float dist = length(L);
        vec3 l = L / dist;

        float ndotl = dot(n, l);

        // Soft terminator
        float diffuse = smoothstep(-0.05, 0.25, ndotl);

        // Atmospheric rim
        float rim = pow(1.0 - max(dot(n, -l), 0.0), 3.0) * 0.35;

        // Physically-inspired falloff
        float attenuation = lightIntensity[i] / (dist * dist + 1.0);

        color += baseColor.rgb * (diffuse + rim) * lightColor[i] * attenuation;
    }

    // Minimal ambient
    color += baseColor.rgb * 0.03;

    // Emmision
    color += baseColor.rgb * emissiveIntensity;

    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    finalColor = vec4(color, baseColor.a);
}
