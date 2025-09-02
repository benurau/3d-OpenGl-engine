#ifndef HITBOX_H
#define HITBOX_H

#include "Geometry3D.h"
#include "misc.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};


class HitBox {
public:
    glm::vec3 min;
    glm::vec3 max;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    glm::mat3 NormalMatrix;
    glm::mat4 ModelMatrix;
    glm::mat4 projection;
    glm::mat4 view;

    CTriangle* cTriangles;
    int cTrianglesCount;
    bool close;

    HitBox() {};
    HitBox(std::vector<Vertex> vertices, std::vector<GLuint> indices) {
        this->vertices = vertices;
        this->indices = indices;      
        this->cTrianglesCount = indices.size()/3;
        cTriangles = new CTriangle[cTrianglesCount];
        PrepareTriangles();
    }

    void Destroy()
    {
        delete[] cTriangles;
        SetDefaults();
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


    void PrepareTriangles()
    {
        CTriangle *Triangle = cTriangles;
        int j = 0;
        for (int i = 0; i < cTrianglesCount; i++)
        {     
            for (int v = 0; v < 3; v++)
            {
                glm::vec3 Vertex = vertices[indices[j]].position;  
                //std::cout << vertices[indices[v]].position.x <<"  "<< vertices[indices[v]].position.y<<"   " << vertices[indices[v]].position.z << "\n";
                glm::vec3 transformed = glm::vec3(ModelMatrix * glm::vec4(Vertex, 1.0f));
                //std::cout << transformed.x <<"  "<< transformed.y<<"   " << transformed.z << "\n";
                Triangle->Vertex[v] = transformed;
                j++;
            }
            Triangle->Normal[0] = normalize(cross(Triangle->Vertex[1] - Triangle->Vertex[0], Triangle->Vertex[2] - Triangle->Vertex[0]));
            Triangle->D[0] = -dot(Triangle->Normal[0], Triangle->Vertex[0]);

            for (int e = 0; e < 3; e++)
            {
                Triangle->Edge[e] = Triangle->Vertex[(e + 1) % 3] - Triangle->Vertex[e];
                Triangle->EdgeLength[e] = length(Triangle->Edge[e]);
                Triangle->Edge[e] /= Triangle->EdgeLength[e];
                Triangle->Normal[1 + e] = cross(Triangle->Edge[e], Triangle->Normal[0]);
                Triangle->D[1 + e] = -dot(Triangle->Normal[1 + e], Triangle->Vertex[e]);
            }
            Triangle++;
        }
        if (cTrianglesCount == 0) return;
        min = max = cTriangles[0].Vertex[0];
        Triangle = cTriangles;

        for (int t = 0; t < cTrianglesCount; t++)
        {
            for (int v = 0; v < 3; v++)
            {
                if (Triangle->Vertex[v].x < min.x) min.x = Triangle->Vertex[v].x;
                if (Triangle->Vertex[v].x > max.x) max.x = Triangle->Vertex[v].x;
                if (Triangle->Vertex[v].y < min.y) min.y = Triangle->Vertex[v].y;
                if (Triangle->Vertex[v].y > max.y) max.y = Triangle->Vertex[v].y;
                if (Triangle->Vertex[v].z < min.z) min.z = Triangle->Vertex[v].z;
                if (Triangle->Vertex[v].z > max.z) max.z = Triangle->Vertex[v].z;
            }
            //std::cout << min.x<< "  " << min.y<< "  "<< max.x << "   " <<max.y<<"\n";
            Triangle++;
        }
        assert(cTrianglesCount * 3 <= indices.size());
        min -= RADIUS;
        max += RADIUS;
    }


    void updateModelMatrix(const glm::mat4x4& ModelMatrix)
    {
        this->ModelMatrix = ModelMatrix;
        this->NormalMatrix = glm::transpose(glm::inverse(glm::mat3x3(ModelMatrix)));
        PrepareTriangles();
    }


    void SetDefaults()
    {
        NormalMatrix = glm::mat3();
        ModelMatrix = glm::mat4();
        cTrianglesCount = 0;
        cTriangles = NULL;
    }

private:   
};




#endif 