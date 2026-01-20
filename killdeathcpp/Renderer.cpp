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











