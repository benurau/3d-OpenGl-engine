
#pragma once
#include "modelHelpers.h"
#include "HitBox.h"

struct Node {
    glm::mat4 localModelMat;
    int glMeshIndex;

    AABB localAABB;
    AABB worldAABB;
};

class tinyModel {
public:
    tinygltf::Model model;
    std::vector<GLTFMaterialGPU> gpuMaterials;
    std::vector<Mesh> glMeshes;
    std::vector<Node> orientationNodes;
    int materialOffset;

    tinyModel(const std::string& path) {
        load_model(path);

        int sceneIndex = model.defaultScene >= 0 ? model.defaultScene : 0;
        const tinygltf::Scene& scene = model.scenes[sceneIndex];
   
        gpuMaterials.resize(model.materials.size());
        for (size_t i = 0; i < model.materials.size(); ++i) {
            gpuMaterials[i] = ParseGLTFMaterial(model, model.materials[i]);
        }
        DebugPrintMaterial(gpuMaterials[0], 0);
        for (int root : scene.nodes) {
            processNode(root, glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)));
        }

    }

private:
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    void load_model(const std::string& path) {
        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
        if (!warn.empty()) std::cout << "Warn: " << warn << std::endl;
        if (!err.empty())  std::cerr << "Err: " << err << std::endl;
        if (!ret) throw std::runtime_error("Failed to load glTF");
    }

    void processNode(int nodeIndex, glm::mat4 parentTransform) {
        const tinygltf::Node& node = model.nodes[nodeIndex];
        glm::mat4 local = getNodeTransform(node);
        glm::mat4 world = parentTransform * local;
        if (node.mesh >= 0) {
            const tinygltf::Mesh& mesh = model.meshes[node.mesh];
            for (const tinygltf::Primitive& primitive : mesh.primitives) {
                createGlMesh(model, primitive, world);
            }
        }
        for (int child : node.children) {
            processNode(child, world);
        }
    }

    void createGlMesh( const tinygltf::Model& model, const tinygltf::Primitive& primitive, glm::mat4& totalTransform) {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        std::vector<glm::vec4> tangents;
        std::vector<glm::uvec4> joints;
        std::vector<glm::vec4> weights;
        std::vector<unsigned int> indices;
        int materialIndex;

        if (primitive.attributes.count("POSITION")) {
            const tinygltf::Accessor& accessor = model.accessors.at(primitive.attributes.at("POSITION"));
            ReadAccessor(model, accessor, positions);
        }
        if (primitive.attributes.count("NORMAL")) {
            const tinygltf::Accessor& accessor = model.accessors.at(primitive.attributes.at("NORMAL"));
            ReadAccessor(model, accessor, normals);
        }
        if (primitive.attributes.count("TEXCOORD_0")) {
            const tinygltf::Accessor& accessor = model.accessors.at(primitive.attributes.at("TEXCOORD_0"));
            ReadAccessor(model, accessor, texcoords);
        }
        if (primitive.attributes.count("TANGENT")) {
            const tinygltf::Accessor& accessor = model.accessors.at(primitive.attributes.at("TANGENT"));
            ReadAccessor(model, accessor, tangents);
        }
        if (primitive.attributes.count("JOINTS_0")) {
            const tinygltf::Accessor& accessor = model.accessors.at(primitive.attributes.at("JOINTS_0"));
            ReadAccessor(model, accessor, joints);
        }
        if (primitive.attributes.count("WEIGHTS_0")) {
            const tinygltf::Accessor& accessor = model.accessors.at(primitive.attributes.at("WEIGHTS_0"));
            ReadAccessor(model, accessor, weights);
        }
        if (primitive.indices >= 0) {
            const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
            ReadAccessor(model, accessor, indices);
        }      
        std::vector<Vertex> vertices = ZipVertices(positions, normals, texcoords, tangents, joints, weights);
        AABB localAABB = computeLocalAABB(vertices);
        Mesh mesh(vertices, indices);
        
        if (primitive.material >= 0) {
            mesh.materialIndex = primitive.material;
        }
        else {
            mesh.materialIndex = -1;
        }
        int meshIndex = static_cast<int>(glMeshes.size());
        glMeshes.push_back(std::move(mesh));

        Node node{};
        node.glMeshIndex = meshIndex;
        node.localModelMat = totalTransform;
        node.localAABB = localAABB;

        orientationNodes.push_back(node);   

    }


    GLTFMaterialGPU ParseGLTFMaterial(const tinygltf::Model& model, const tinygltf::Material& mat){
        GLTFMaterialGPU out;
        const auto& pbr = mat.pbrMetallicRoughness;
        out.baseColorFactor = stdVec4ToGlm(pbr.baseColorFactor);
        if (pbr.baseColorTexture.index >= 0) {
            const auto& tex = model.textures[pbr.baseColorTexture.index];
            const auto& img = model.images[tex.source];
            out.baseColorTex = TextureFromGLTFImage(img, true); // sRGB
            ApplyGLTFSampler(out.baseColorTex.id, model, tex);
        }
        out.metallicFactor = static_cast<float>(pbr.metallicFactor);
        out.roughnessFactor = static_cast<float>(pbr.roughnessFactor);
        if (pbr.metallicRoughnessTexture.index >= 0) {
            const auto& tex = model.textures[pbr.metallicRoughnessTexture.index];
            const auto& img = model.images[tex.source];
            out.metallicRoughnessTex = TextureFromGLTFImage(img, false);
            ApplyGLTFSampler(out.metallicRoughnessTex.id, model, tex);
        }
        out.normalScale = static_cast<float>(mat.normalTexture.scale);
        if (mat.normalTexture.index >= 0) {
            const auto& tex = model.textures[mat.normalTexture.index];
            const auto& img = model.images[tex.source];
            out.normalTex = TextureFromGLTFImage(img, false);
            ApplyGLTFSampler(out.normalTex.id, model, tex);
        }
        if (mat.occlusionTexture.index >= 0) {
            const auto& tex = model.textures[mat.occlusionTexture.index];
            const auto& img = model.images[tex.source];
            out.occlusionTex = TextureFromGLTFImage(img, false);
            ApplyGLTFSampler(out.occlusionTex.id, model, tex);
        }
        out.emissiveFactor = stdVec3ToGlm(mat.emissiveFactor);
        if (mat.emissiveTexture.index >= 0) {
            const auto& tex = model.textures[mat.emissiveTexture.index];
            const auto& img = model.images[tex.source];
            out.emissiveTex = TextureFromGLTFImage(img, true); // sRGB
            ApplyGLTFSampler(out.emissiveTex.id, model, tex);
        }
        out.doubleSided = mat.doubleSided;
        return out;
    }

    std::vector<Vertex> ZipVertices(
        const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec3>& normals,
        const std::vector<glm::vec2>& texcoords,
        const std::vector<glm::vec4>& tangents,
        const std::vector<glm::uvec4>& joints,
        const std::vector<glm::vec4>& weights)
    {
        size_t vertexCount = positions.size();
        std::vector<Vertex> vertices(vertexCount);

        for (size_t i = 0; i < vertexCount; ++i) {
            Vertex& v = vertices[i];
            v.position = positions[i];
            v.normal = normals.empty() ? glm::vec3(0, 1, 0) : normals[i];
            v.texCoords = texcoords.empty() ? glm::vec2(0) : texcoords[i];
            v.Tangent = tangents.empty() ? glm::vec4(0)  : tangents[i];
            v.joints = joints.empty() ? glm::uvec4(0) : joints[i];
            v.weights = weights.empty() ? glm::vec4(0) : weights[i];
        }
        return vertices;
    }


};
