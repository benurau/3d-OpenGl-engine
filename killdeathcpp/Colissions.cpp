#include "Colissions.h"

bool point_Box_Colission(HitBox& box, glm::vec3 position, glm::vec3& Movement)
{
    glm::vec3 newPosition = position + Movement;
    float EPSILON = 0.000001f;
    box.close = false;
    if (newPosition.x < box.min.x) return false;
    //std::cout << min.x << "  " << position.x << "first\n";
    if (newPosition.x > box.max.x) return false;
    //std::cout << max.x << "  " << position.x << "first1\n";
    if (newPosition.y < box.min.y) return false;
    //std::cout << min.y << "  " << position.y << "first2\n";
    if (newPosition.y > box.max.y) return false;
    //std::cout <<  max.y << "  " << position.y << "first3\n";
    if (newPosition.z < box.min.z) return false;
    //std::cout << min.z << "  " << position.z << "first4\n";           
    if (newPosition.z > box.max.z) return false;
    //std::cout << max.z << "  " << position.z << "first5\n";
    box.close = true;
    CTriangle* cTriangle = box.cTriangles;
    for (int t = 0; t < box.cTrianglesCount; t++)
    {
        float Distance = glm::dot(cTriangle->Normal[0], newPosition) + cTriangle->D[0];
        cTriangle->close = Distance > 0.0f && Distance < RADIUS;
        if (cTriangle->close)
        {
            if (dot(cTriangle->Normal[1], newPosition) + cTriangle->D[1] < 0.0f)
            {
                if (dot(cTriangle->Normal[2], newPosition) + cTriangle->D[2] < 0.0f)
                {
                    if (dot(cTriangle->Normal[3], newPosition) + cTriangle->D[3] < 0.0f)
                    {
                        float offset = RADIUS - Distance;
                        if (offset < EPSILON) return false;
                        //printf("%d depth %f distance\n", depth, Distance);
                        //printf("%d depth %f offset\n", depth, offset);
                        //printf("%f move.x hitbox first \n", Movement.x, Movement.y, Movement.z);
                        //printf("normal %f x %f y %f z \n", cTriangle->Normal[0].x, cTriangle->Normal[0].y, cTriangle->Normal[0].z);
                        Movement += cTriangle->Normal[0] * offset;
                        //printf("%f move.x hitbox second \n", Movement.x, Movement.y, Movement.z);
                        return true;
                    }
                }
            }
        }

        cTriangle++;
    }

    // check the distance of the camera position from each edge
    if (!box.close) return false;
    CTriangle* Triangle = box.cTriangles;
    for (int t = 0; t < box.cTrianglesCount; t++)
    {
        if (Triangle->close)
        {
            for (int e = 0; e < 3; e++)
            {
                glm::vec3 VCP = newPosition - Triangle->Vertex[e];
                float EdotVCP = dot(Triangle->Edge[e], VCP);
                if (EdotVCP > 0.0f && EdotVCP < Triangle->EdgeLength[e])
                {
                    glm::vec3 Normal = VCP - Triangle->Edge[e] * EdotVCP;
                    float Distance = length(Normal);
                    if (Distance > 0.0f && Distance < RADIUS)
                    {
                        Movement += Normal * ((RADIUS) / Distance - 1.0f);
                        return true;
                    }
                }
            }
        }

        Triangle++;
    }

    // check the distance of the camera position from each vertex
    if (box.close) return false;
    Triangle = box.cTriangles;
    for (int t = 0; t < box.cTrianglesCount; t++)
    {
        if (Triangle->close)
        {
            for (int v = 0; v < 3; v++)
            {
                glm::vec3 Normal = newPosition - Triangle->Vertex[v];
                float Distance = length(Normal);
                std::cout << "cameradistancefromcertex" << Distance << "\n";
                //std::cout << "camertadisntancefromvertex" << Distance;
                if (Distance > 0.0f && Distance < RADIUS)
                {
                    Movement += Normal * ((RADIUS) / Distance - 1.0f);
                    return true;
                }
            }
        }

        Triangle++;
    }
    return false;
}


std::vector<glm::vec3> computeVertexNormals(
    const std::vector<glm::vec3>& positions,
    const std::vector<unsigned int>& indices
) {
    std::vector<glm::vec3> normals(positions.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        glm::vec3 v0 = positions[i0];
        glm::vec3 v1 = positions[i1];
        glm::vec3 v2 = positions[i2];
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
        normals[i0] += faceNormal;
        normals[i1] += faceNormal;
        normals[i2] += faceNormal;
    }

    for (auto& n : normals) {
        n = glm::normalize(n);
        printf("%f x %f y %f z normals after normalization \n", n.x, n.y, n.z);
    }
    return normals;
}