#version 330

in vec3 worldPos;

uniform vec3 cameraPos;
uniform vec3 atmoColor;
uniform vec3 planetCenter;
uniform vec3 lightDir;

uniform float planetRadius;
uniform float atmoThickness;
uniform float intensity;

out vec4 fragColor;

bool raySphere(vec3 ro, vec3 rd, float r, out float t0, out float t1)
{
    float b = dot(ro, rd);
    float c = dot(ro, ro) - r * r;
    float h = b * b - c;
    if (h < 0.0) return false;
    h = sqrt(h);
    t0 = -b - h;
    t1 = -b + h;
    return true;
}

float sampleAtmosphere(vec3 ro, vec3 rd)
{
    float t0, t1;
    float outerRadius = planetRadius + atmoThickness;

    if (!raySphere(ro, rd, outerRadius, t0, t1))
        return 0.0;

    t0 = max(t0, 0.0);

    const int STEPS = 8;
    float stepSize = (t1 - t0) / float(STEPS);

    float opticalDepth = 0.0;

    for (int i = 0; i < STEPS; i++)
    {
        float t = t0 + stepSize * (float(i) + 0.5);
        vec3 p = ro + rd * t;

        float height = length(p - planetCenter) - planetRadius;
        float h = clamp(height / atmoThickness, 0.0, 1.0);

        float density = exp(-h * 4.0);
        opticalDepth += density * stepSize;
    }

    return opticalDepth;
}

float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

void main()
{
    vec3 ro = cameraPos - planetCenter; 
    vec3 rd = normalize(worldPos - cameraPos);
    float opticalDepth = sampleAtmosphere(ro, rd);

    vec3 normal  = normalize(worldPos - planetCenter);
    vec3 viewDir = normalize(worldPos - cameraPos);

    float NdotL     = dot(normal, lightDir);
    float sunAmount = clamp(NdotL * 0.5 + 0.5, 0.0, 1.0);

    float viewAmount  = pow(1.0 - max(dot(normal, -viewDir), 0.0), 3.0);
    float backScatter = pow(1.0 - abs(NdotL), 2.0);
    float scattering  = (sunAmount * 0.7 + backScatter * 0.3) * viewAmount;

    float alpha = (1.0 - exp(-opticalDepth * intensity)) * scattering;
    alpha = clamp(alpha, 0.0, 1.0);

    vec3 color = atmoColor * alpha;
    fragColor = vec4(color, alpha);
}
