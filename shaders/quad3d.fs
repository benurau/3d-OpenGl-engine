#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform bool useTexture;

void main()
{
    if (useTexture){
        FragColor = texture(texture_diffuse1, TexCoord);
    }
    else
    {
        FragColor = vec4(1.0);
    }
}
