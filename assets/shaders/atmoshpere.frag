#version 330

in vec3 worldPos;

uniform vec3 cameraPos;
uniform vec3 atmoColor;

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

        float height = length(p) - planetRadius;
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
    vec3 ro = cameraPos;
    vec3 rd = normalize(worldPos - cameraPos);

    float opticalDepth = sampleAtmosphere(ro, rd);

    float alpha = 1.0 - exp(-opticalDepth * intensity);
    alpha = clamp(alpha, 0.0, 0.6);

    vec3 normal = normalize(worldPos);
    float horizon = pow(1.0 - saturate(dot(normal, -rd)), 2.5);

    vec3 color = atmoColor * horizon;

    fragColor = vec4(color, alpha);
}

