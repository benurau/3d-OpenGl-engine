#ifndef OBJECTPARAMS_H
#define OBJECTPARAMS_H



float line[]{
        -0.3f, -0.3f,  0.0f,
        0.3f, 0.3f,  0.0f
};

std::vector<glm::vec3> cubePos = {
        {0.0f, -0.25f, 0.5f}, {0.5f, -0.25f, 0.5f}, {0.5f, 0.25f,  0.5f}, {0.0f, 0.25f, 0.5f},
        {0.0f, -0.25f, 0.0f}, {0.5f, -0.25f, 0.0f}, {0.5f, 0.25f, 0.0f}, {0.0f, 0.25f, 0.0f}
};

std::vector<glm::vec2> texCoords = {
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
};


std::vector<glm::vec3> normals = {
    { -1.0f, -1.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f },
    { -1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f }
};

std::vector<unsigned int> cubeIndices = {
    0, 1, 2,  2, 3, 0,
    6, 5, 4,  4, 7, 6,
    4, 0, 3,  3, 7, 4,
    1, 5, 6,  6, 2, 1,
    3, 2, 6,  6, 7, 3,
    4, 5, 1,  1, 0, 4
};

std::vector<glm::vec3> quadPos = {
    {-0.5f, -0.5f, 0.0f}, // Bottom-left corner
    { 0.5f, -0.5f, 0.0f}, // Bottom-right corner
    { 0.5f,  0.5f, 0.0f}, // Top-right corner
    {-0.5f,  0.5f, 0.0f}  // Top-left corner
};

std::vector<glm::vec2> quadTexCoords = {
    {0.0f, 0.0f}, // Bottom-left corner
    {1.0f, 0.0f}, // Bottom-right corner
    {1.0f, 1.0f}, // Top-right corner
    {0.0f, 1.0f}  // Top-left corner
};

std::vector<glm::vec3> quadNormals(4, { 0.0f, 0.0f, 1.0f });  // Normal pointing in the Z-axis (for 2D quad)

std::vector<unsigned int> quadIndices = {
    0, 1, 2, // First triangle (bottom-left, bottom-right, top-right)
    2, 3, 0  // Second triangle (top-right, top-left, bottom-left)
};

std::vector<Vertex> triangleVertices = {
    { { 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f, 0.8f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
    { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} }
};
#endif