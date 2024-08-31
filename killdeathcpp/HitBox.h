#include "Geometry3D.h"
#include "shader.h"


struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};


class HitBox {
public:
    HitBox() {};
    HitBox(std::vector<Vertex> vertices, std::vector<GLuint> indices, Shader shader) {
        this->shader = shader;
        for (int i = 0; i < indices.size(); i++)
        {
            glm::vec3 a = vertices[indices[i]].position;
            Vertex& aV = vertices[indices[i]];
            i++;
            glm::vec3 b = vertices[indices[i]].position;
            Vertex& bV = vertices[indices[i]];
            i++;
            glm::vec3 c = vertices[indices[i]].position;
            Vertex& cV = vertices[indices[i]];
            Triangle tri = { a, b, c, aV, bV, cV };
            this->triangles.push_back(tri);
        }
    }

    bool checkAllPointsInTriangles(glm::vec3 point) {
        for (Triangle t : triangles) {
            if (checkPointInTriangle(point, t)) {
                return true;
            }         
        }
        return false;
    }

    bool checkPointInTriangle(glm::vec3 point, const Triangle& triangle)
    {
        glm::vec3 a = triangle.a - point;
        glm::vec3 b = triangle.b - point;
        glm::vec3 c = triangle.c - point;

        glm::vec3 normPBC = glm::cross(b, c);
        glm::vec3 normPCA = glm::cross(c, a);
        glm::vec3 normPAB = glm::cross(a, b);

        if (glm::dot(normPBC, normPCA) < 0.0f) {
            return false;
        }
        else if (glm::dot(normPBC, normPAB) < 0.0f) {
            return false;
        }
        return true;
    }

private:
    std::vector<Triangle> triangles;
    Shader shader;
};


