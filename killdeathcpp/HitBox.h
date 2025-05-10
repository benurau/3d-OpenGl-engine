#include "Geometry3D.h"
#include "shader.h"
#define RADIUS 0.15f

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

    glm::vec3 correctingMovement;

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
                glm::vec3 transformed = glm::vec4(Vertex, 1.0f) * ModelMatrix;
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

        min -= RADIUS;
        max += RADIUS;
    }

    void setModelMatrix(const glm::mat4x4& ModelMatrix)
    {
        this->ModelMatrix = ModelMatrix;
        this->NormalMatrix = glm::transpose(glm::inverse(glm::mat3x3(ModelMatrix)));
        PrepareTriangles();
    }
    void setViewMatrix(const glm::mat4x4& view)
    {
        this->view = view;
        PrepareTriangles();
    }
    void setProjectionMatrix(const glm::mat4x4& projection)
    {
        this->projection = projection;
        PrepareTriangles();
    }

    bool CheckCameraCollision(glm::vec3 position, glm::vec3 Movement, int depth = 0)
    {
        if (depth < 4) {
            close = false;
            if (position.x < min.x) return false;
            //std::cout << min.x << "  " << position.x << "first\n";
            if (position.x > max.x) return false;
            //std::cout << max.x << "  " << position.x << "first1\n";
            if (position.y < min.y) return false;
            //std::cout << min.y << "  " << position.y << "first2\n";
            if (position.y > max.y) return false;
            //std::cout <<  max.y << "  " << position.y << "first3\n";
            if (position.z < min.z) return false;
            //std::cout << min.z << "  " << position.z << "first4\n";           
            if (position.z > max.z) return false;
            //std::cout << max.z << "  " << position.z << "first5\n";
            close = true;
            CTriangle* cTriangle = cTriangles;
            for (int t = 0; t < cTrianglesCount; t++)
            {
                float Distance = glm::dot(cTriangle->Normal[0], position) + cTriangle->D[0];   
                //std::cout << "camertadisntance"<< t << Distance << "\n";
                cTriangle->close = Distance > 0.0f && Distance < RADIUS;
                if (cTriangle->close)
                {
                    if (dot(cTriangle->Normal[1], position) + cTriangle->D[1] < 0.0f)
                    {
                        if (dot(cTriangle->Normal[2], position) + cTriangle->D[2] < 0.0f)
                        {
                            if (dot(cTriangle->Normal[3], position) + cTriangle->D[3] < 0.0f)
                            {
                                //printf("%f, %f, %f \n", cTriangle->Normal[0].x, cTriangle->Normal[0].y, cTriangle->Normal[0].z);
                                Movement += cTriangle->Normal[0] * (RADIUS - Distance);
                                correctingMovement = Movement;
                                CheckCameraCollision(position, Movement, depth + 1);
                                return true;
                            }
                        }
                    }
                }

                cTriangle++;
            }

            // check the distance of the camera position from each edge
            if (!close) return false;
            CTriangle* Triangle = cTriangles;
            for (int t = 0; t < cTrianglesCount; t++)
            {
                if (Triangle->close)
                {
                    for (int e = 0; e < 3; e++)
                    {
                        glm::vec3 VCP = position - Triangle->Vertex[e];
                        float EdotVCP = dot(Triangle->Edge[e], VCP);
                        if (EdotVCP > 0.0f && EdotVCP < Triangle->EdgeLength[e])
                        {
                            glm::vec3 Normal = VCP - Triangle->Edge[e] * EdotVCP;
                            float Distance = length(Normal);
                            std::cout << "camertadisntancefromedge" << Distance <<"\n";
                            if (Distance > 0.0f && Distance < RADIUS)
                            {
                                Movement += Normal * (RADIUS / Distance - 1.0f);
                                correctingMovement = Movement;
                                CheckCameraCollision(position, Movement, depth + 1);
                                return true;
                            }
                        }
                    }
                }

                Triangle++;
            }           

            // check the distance of the camera position from each vertex
            if (close) return false;
            Triangle = cTriangles;
            for (int t = 0; t < cTrianglesCount; t++)
            {
                if (Triangle->close)
                {
                    for (int v = 0; v < 3; v++)
                    {
                        glm::vec3 Normal = position - Triangle->Vertex[v];
                        float Distance = length(Normal);
                        std::cout << "cameradistancefromcertex" << Distance << "\n";
                        //std::cout << "camertadisntancefromvertex" << Distance;
                        if (Distance > 0.0f && Distance < RADIUS)
                        {
                            Movement += Normal * (RADIUS / Distance - 1.0f);
                            correctingMovement = Movement;
                            CheckCameraCollision(position, Movement, depth + 1);
                            return true;
                        }
                    }
                }

                Triangle++;
            }           
        }
        return false;
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




