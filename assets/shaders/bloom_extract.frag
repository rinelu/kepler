#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float threshold;

void main()
{
    vec3 color = texture(texture0, fragTexCoord).rgb;

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    float knee = threshold * 0.5;
    float soft = smoothstep(threshold - knee, threshold + knee, brightness);

    finalColor = vec4(color * soft * 1.5, 1.0);
}
