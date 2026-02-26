#include "renderer.h"
#include <cmath>




Renderer::Renderer(GLFWwindow* window) : window(window){}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
}

Material Renderer::ConvertGLTFMaterialToMaterial(const GLTFMaterialGPU& src, Shader* shader)
{
    Material mat(shader);
    mat.vec3Uniforms["uEmissiveFactor"] = src.emissiveFactor;
    mat.floatUniforms["uMetallicFactor"] = src.metallicFactor;
    mat.floatUniforms["uRoughnessFactor"] = src.roughnessFactor;
    mat.floatUniforms["uNormalScale"] = src.normalScale;
    mat.vec4Uniforms["uBaseColorFactor"] = src.baseColorFactor;
    if (src.baseColorTex.id != 0)
        mat.textureUniforms["uBaseColorTex"] = src.baseColorTex;
    if (src.metallicRoughnessTex.id != 0)
        mat.textureUniforms["uMetallicRoughnessTex"] = src.metallicRoughnessTex;
    if (src.normalTex.id != 0)
        mat.textureUniforms["uNormalTex"] = src.normalTex;
    if (src.occlusionTex.id != 0)
        mat.textureUniforms["uOcclusionTex"] = src.occlusionTex;
    if (src.emissiveTex.id != 0)
        mat.textureUniforms["uEmissiveTex"] = src.emissiveTex;
    mat.floatUniforms["uDoubleSided"] = src.doubleSided ? 1.0f : 0.0f;

    return mat;
}


void Renderer::drawModel(tinyModel& model, ObjectOrientation& rootOrientation) {
    for (const Node& node : model.nodes) {
        if (node.glMeshIndex < 0) continue;

        ObjectOrientation finalOrientation = rootOrientation;

        finalOrientation.modelMatrix = rootOrientation.modelMatrix * node.globalMatrix;
       
        Mesh& mesh = model.glMeshes[node.glMeshIndex];
        
        uint32_t matIndex = mesh.materialIndex + model.materialOffset;
        if (matIndex >= materials.size()) {
            std::cerr << "[Renderer] Invalid material index\n";
            continue;
        }
        Material& mat = materials[matIndex];

        mat.shader->use();
        if (node.skinIndex >= 0) {
            Skin& skin = model.skins[node.skinIndex];
            mat.shader->setMat4Array( "jointMatrices", skin.jointMatrices);
            mat.shader->setBool("skinned", 1);
        }
        else {
            mat.shader->setBool("skinned", 0);
        }
        //model.updateSkinsDiagnostics();
        draw(mesh, finalOrientation, mat);
    }
}



void Renderer::draw(Mesh& mesh, ObjectOrientation& orientation, Material& material){
    if (!material.shader) {
        std::cerr << "[Draw] Error: Material has no shader.\n";
        return;
    }

    material.shader->setObjectOrientation(orientation);
    checkGLError("setMat4(model)");

    material.apply();
    checkGLError("material.apply");
    //std::cout << "[Draw] Binding VAO ID: " << mesh.vao << std::endl;
    //orientation.debugPrint();
    //orientation.debugPrint();
    glBindVertexArray(mesh.vao);
    checkGLError("glBindVertexArray");
    //material.shader->PrintDebugUniforms();
    if (!mesh.indices.empty()) {
        //std::cout << "[Draw] Drawing with glDrawElements, count: " << mesh.indices.size() << std::endl;
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        checkGLError("glDrawElements");
    }
    else {
        //std::cout << "[Draw] Drawing with glDrawArrays, count: " << mesh.totalVerticles << std::endl;
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
        checkGLError("glDrawArrays");
    }
    glBindVertexArray(0);
}


//debug functions

void Renderer::drawLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color, const glm::mat4& viewProj, Shader& shader)
{
    static GLuint vao = 0;
    static GLuint vbo = 0;

    if (vao == 0)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }

    glm::vec3 points[2] = { a, b };
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    shader.use();
    shader.setMat4("uViewProj", viewProj);
    shader.setVec3("uColor", color);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Renderer::drawCircle(const glm::vec3& center,const glm::vec3& normal, float radius,const glm::vec3& color, const glm::mat4& viewProj, Shader& shader){
    const int segments = 32;
    glm::vec3 tangent = glm::normalize(glm::cross(normal, glm::vec3(0, 1, 0)));
    if (glm::length(tangent) < 0.01f)
        tangent = glm::normalize(glm::cross(normal, glm::vec3(1, 0, 0)));
    glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));
    for (int i = 0; i < segments; ++i)
    {
        float a0 = (float)i / segments * glm::two_pi<float>();
        float a1 = (float)(i + 1) / segments * glm::two_pi<float>();
        glm::vec3 p0 = center + (cos(a0) * tangent + sin(a0) * bitangent) * radius;
        glm::vec3 p1 = center + (cos(a1) * tangent + sin(a1) * bitangent) * radius;
        drawLine(p0, p1, color, viewProj, shader);
    }
}

void Renderer::drawAABB(const AABB& box,const glm::mat4& viewProj, const glm::vec3& color, Shader& shader)
{
    const glm::vec3& min = box.min;
    const glm::vec3& max = box.max;

    glm::vec3 corners[8] =
    {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, max.y, min.z},
        {min.x, max.y, min.z},

        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };

    drawLine(corners[0], corners[1], color, viewProj, shader);
    drawLine(corners[1], corners[2], color, viewProj, shader);
    drawLine(corners[2], corners[3], color, viewProj, shader);
    drawLine(corners[3], corners[0], color, viewProj, shader);
    drawLine(corners[4], corners[5], color, viewProj, shader);
    drawLine(corners[5], corners[6], color, viewProj, shader);
    drawLine(corners[6], corners[7], color, viewProj, shader);
    drawLine(corners[7], corners[4], color, viewProj, shader);
    drawLine(corners[0], corners[4], color, viewProj, shader);
    drawLine(corners[1], corners[5], color, viewProj, shader);
    drawLine(corners[2], corners[6], color, viewProj, shader);
    drawLine(corners[3], corners[7], color, viewProj, shader);
}


void Renderer::drawCapsule(const CapsuleWorldLoc& c,const glm::mat4& viewProj,const glm::vec3& color, Shader& shader)
{
    const int segments = 24;

    glm::vec3 axis = c.p1 - c.p0;
    float axisLen = glm::length(axis);

    if (axisLen < 0.0001f)
        return;

    axis /= axisLen;

    glm::vec3 tangent = glm::cross(axis, glm::vec3(0, 1, 0));
    if (glm::length(tangent) < 0.001f)
        tangent = glm::cross(axis, glm::vec3(1, 0, 0));

    tangent = glm::normalize(tangent);
    glm::vec3 bitangent = glm::normalize(glm::cross(axis, tangent));

    drawLine(c.p0, c.p1, glm::vec3(1, 0, 0), viewProj, shader);

    for (int i = 0; i < segments; ++i)
    {
        float a0 = (float)i / segments * glm::two_pi<float>();
        float a1 = (float)(i + 1) / segments * glm::two_pi<float>();

        glm::vec3 offset0 = (cos(a0) * tangent + sin(a0) * bitangent) * c.radius;
        glm::vec3 offset1 = (cos(a1) * tangent + sin(a1) * bitangent) * c.radius;

        drawLine(c.p0 + offset0, c.p0 + offset1, color, viewProj, shader);
        drawLine(c.p1 + offset0, c.p1 + offset1, color, viewProj, shader);
    }
    drawLine(c.p0 + tangent * c.radius, c.p1 + tangent * c.radius, color, viewProj, shader);
    drawLine(c.p0 - tangent * c.radius, c.p1 - tangent * c.radius, color, viewProj, shader);
    drawLine(c.p0 + bitangent * c.radius, c.p1 + bitangent * c.radius, color, viewProj, shader);
    drawLine(c.p0 - bitangent * c.radius, c.p1 - bitangent * c.radius, color, viewProj, shader);
    for (int i = 0; i < segments; ++i)
    {
        float a0 = (float)i / segments * glm::pi<float>();
        float a1 = (float)(i + 1) / segments * glm::pi<float>();

        glm::vec3 ring0 = cos(a0) * axis * c.radius;
        glm::vec3 ring1 = cos(a1) * axis * c.radius;

        glm::vec3 side0 = sin(a0) * tangent * c.radius;
        glm::vec3 side1 = sin(a1) * tangent * c.radius;

        drawLine(c.p0 + ring0 + side0, c.p0 + ring1 + side1, color, viewProj, shader);
        drawLine(c.p1 - ring0 + side0, c.p1 - ring1 + side1, color, viewProj, shader);
    }
}





