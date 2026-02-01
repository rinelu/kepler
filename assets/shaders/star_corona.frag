#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in float shellRadius;

uniform vec3 glowColor;
uniform float intensity;
uniform vec3 viewPos;

out vec4 finalColor;

void main()
{
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 normal  = normalize(-fragNormal);

    float fresnel = 1.0 - max(dot(normal, viewDir), 0.0);
    fresnel = pow(fresnel, 2.5);

    float radial = smoothstep(0.6, 1.0, shellRadius);
    radial = pow(radial, 1.5);

    float glow = fresnel * radial * intensity;
    vec3 color = glowColor * glow * 6.0;
    float alpha = glow * 0.8;

    finalColor = vec4(color, alpha);
}
