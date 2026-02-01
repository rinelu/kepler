#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 texelSize;  // 1.0 / texture resolution
uniform vec2 direction; // (1,0) horizontal, (0,1) vertical

void main()
{
    vec3 result = vec3(0.0);

    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    result += texture(texture0, fragTexCoord).rgb * weights[0];

    for (int i = 1; i < 5; i++)
    {
        vec2 offset = direction * texelSize * float(i);
        result += texture(texture0, fragTexCoord + offset).rgb * weights[i];
        result += texture(texture0, fragTexCoord - offset).rgb * weights[i];
    }

    finalColor = vec4(result, 1.0);
}
