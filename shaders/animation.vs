#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Maximum number of bones supported
const int MAX_BONES = 100;

// Bone transformation matrices
uniform mat4 bones[MAX_BONES];

void main()
{
    // Skinning calculation: blend bone transforms
    mat4 skinMatrix = aWeights[0] * bones[aBoneIDs[0]] +
                      aWeights[1] * bones[aBoneIDs[1]] +
                      aWeights[2] * bones[aBoneIDs[2]] +
                      aWeights[3] * bones[aBoneIDs[3]];

    vec4 skinnedPosition = skinMatrix * vec4(aPos, 1.0);

    gl_Position = projection * view * model * skinnedPosition;
    TexCoords = aTexCoords;
}