#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 biTangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Maximum number of bones supported
const int MAX_BONES = 200;
const MAX_BONE_INFLUENCE = 4;

// Bone transformation matrices
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(aBoneIDs[i] == -1) 
            continue;
        if(aBoneIDs[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[aBoneIDs[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * aWeights[i];
        vec3 localNormal = mat3(finalBonesMatrices[aBoneIDs[i]]) * aNormal;
   }
	
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;
	TexCoords = aTexCoords;
}

