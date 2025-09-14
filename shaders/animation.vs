#version 330 core

layout (location = 0) in vec3 aPos;    
layout (location = 1) in vec3 aNormal;   
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs; 
layout (location = 4) in vec4 aWeights;  

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 boneTransforms[100]; // MAX_BONES = 100 (adjust to your needs)

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    // Skinning: blend vertex with bone transforms
    mat4 boneTransform = mat4(0.0);
    boneTransform += boneTransforms[aBoneIDs[0]] * aWeights[0];
    boneTransform += boneTransforms[aBoneIDs[1]] * aWeights[1];
    boneTransform += boneTransforms[aBoneIDs[2]] * aWeights[2];
    boneTransform += boneTransforms[aBoneIDs[3]] * aWeights[3];

    vec4 skinnedPos = boneTransform * vec4(aPos, 1.0);
    vec3 skinnedNormal = mat3(boneTransform) * aNormal;

    FragPos = vec3(model * skinnedPos);
    Normal = normalize(mat3(transpose(inverse(model))) * skinnedNormal);
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
