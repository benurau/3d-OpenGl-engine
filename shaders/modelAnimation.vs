#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 5) in uvec4 joints;
layout(location = 6) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool skinned;
uniform mat4 jointMatrices[128];

out vec2 vTexcoord;

void main()
{
    vec4 pos = vec4(position, 1.0);

    if (skinned) {
        mat4 skinMat =
              weights.x * jointMatrices[joints.x]
            + weights.y * jointMatrices[joints.y]
            + weights.z * jointMatrices[joints.z]
            + weights.w * jointMatrices[joints.w];

        pos = skinMat * pos;
    }

    gl_Position = projection * view * model * pos;
    vTexcoord = texcoord; // pass-through
}
