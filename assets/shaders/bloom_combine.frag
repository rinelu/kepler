#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D sceneTex;
uniform sampler2D bloomTex;
uniform float bloomIntensity;

void main()
{
    vec3 scene = texture(sceneTex, fragTexCoord).rgb;
    vec3 bloom = texture(bloomTex, fragTexCoord).rgb;

    vec3 color = scene + bloom * bloomIntensity;

    color = color / (color + vec3(1.0));

    color = pow(color, vec3(1.0 / 2.2));

    finalColor = vec4(color, 1.0);
}
