#ifndef OBJECTPARAMS_H
#define OBJECTPARAMS_H



inline float line[]{
        -0.3f, -0.3f,  0.0f,
        0.3f, 0.3f,  0.0f
};

inline std::vector<glm::vec3> cubePos = {
        {0.0f, -0.25f, 0.5f}, {0.5f, -0.25f, 0.5f}, {0.5f, 0.25f,  0.5f}, {0.0f, 0.25f, 0.5f},
        {0.0f, -0.25f, 0.0f}, {0.5f, -0.25f, 0.0f}, {0.5f, 0.25f, 0.0f}, {0.0f, 0.25f, 0.0f}
};

inline std::vector<glm::vec2> texCoords = {
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
};


inline std::vector<glm::vec3> normals = {
    { -1.0f, -1.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f },
    { -1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f }
};

inline std::vector<unsigned int> cubeIndices = {
    0, 1, 2,  2, 3, 0,
    6, 5, 4,  4, 7, 6,
    4, 0, 3,  3, 7, 4,
    1, 5, 6,  6, 2, 1,
    3, 2, 6,  6, 7, 3,
    4, 5, 1,  1, 0, 4
};

inline std::vector<glm::vec3> quadPos = {
    {-0.5f, -0.5f, 0.0f},
    { 0.5f, -0.5f, 0.0f},
    { 0.5f,  0.5f, 0.0f},
    {-0.5f,  0.5f, 0.0f}
};

inline std::vector<glm::vec2> quadTexCoords = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f}
};

inline std::vector<glm::vec3> quadNormals(4, { 0.0f, 0.0f, 1.0f });

inline std::vector<unsigned int> quadIndices = {
    0, 1, 2,
    2, 3, 0
};

inline std::vector<Vertex> triangleVertices = {
    { { 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f, 0.8f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
    { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} }
};


#endif