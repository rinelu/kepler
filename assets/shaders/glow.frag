#version 330

in vec3 fragNormal;
in vec3 fragWorldPos;

uniform vec3 viewPos;
uniform vec3 glowColor; // Set this to Yellow (1.0, 0.9, 0.0)
uniform float intensity;

out vec4 finalColor;

void main()
{
    vec3 n = normalize(fragNormal);
    vec3 v = normalize(viewPos - fragWorldPos);

    // dotNV is 1.0 at the center (facing you) and 0.0 at the edges
    float dotNV = max(dot(n, v), 0.0);

    // Create a falloff for the transparency
    // As we move toward the edge, alpha drops
    float alpha = pow(dotNV, 0.5); // Using a low power makes the edges softer

    // Optional: Add a rim highlight to make the edge 'pop' before it fades
    float rim = pow(1.0 - dotNV, 2.0);

    // Mix the star color with the rim for a "hot" edge
    vec3 color = glowColor + (rim * 0.5);

    // Final color with alpha for transparency
    // The star will be solid yellow in the middle and fade at the edges
    finalColor = vec4(color * intensity, alpha);
}
