#version 330 core
out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D uBaseColorTex;
uniform vec4 uBaseColorFactor;

void main()
{
    vec4 baseColor = texture(uBaseColorTex, vTexCoords) * uBaseColorFactor;
    FragColor = baseColor;
}
