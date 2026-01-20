#version 330 core

in vec2 vTexcoord;
out vec4 FragColor;

uniform sampler2D uBaseColorTex;
uniform vec4 uBaseColorFactor;

void main()
{
    vec4 baseColor = texture(uBaseColorTex, vTexcoord) * uBaseColorFactor;
    FragColor = baseColor;
}