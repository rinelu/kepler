#version 330

in vec2 fragUV;

uniform vec4 outlineColor;
uniform float time;

out vec4 finalColor;

float inner = 0.965;
float outer = 0.99;

void main()
{
    vec2 uv = fragUV * 2.0 - 1.0;
    float r = length(uv);

    if (r < inner || r > outer) discard;

    float angle = atan(uv.y, uv.x);
    float stripes = sin(angle * 8.0 + time * 1.0);

    if (stripes < 0.0) discard;

    float alpha = smoothstep(inner, inner + 0.02, r) * smoothstep(outer, outer - 0.02, r);

    finalColor = vec4(outlineColor.rgb, outlineColor.a * alpha);
}
