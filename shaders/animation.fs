#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D uTextureDiffuse;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

void main()
{
    // Simple diffuse + specular lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * texture(uTextureDiffuse, TexCoords).rgb;

    // Specular
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec;

    vec3 result = diffuse + specular;
    FragColor = vec4(result, 1.0);
}
