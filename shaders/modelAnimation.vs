#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];

out vec2 TexCoords;

void main()
{
    // skinning
    mat4 skinMatrix = bones[aBoneIDs[0]] * aWeights[0];
    skinMatrix += bones[aBoneIDs[1]] * aWeights[1];
    skinMatrix += bones[aBoneIDs[2]] * aWeights[2];
    skinMatrix += bones[aBoneIDs[3]] * aWeights[3];

    vec4 localPos = skinMatrix * vec4(aPos, 1.0);
    gl_Position = projection * view * model * localPos;

    TexCoords = aTexCoords;
}
