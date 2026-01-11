
#pragma once
#include "modelHelpers.h"
#include "HitBox.h"

struct Node {
    int parent = -1;
    std::vector<int> children;

    glm::vec3 translation{ 0.0f };
    glm::quat rotation{ 1, 0, 0, 0 };
    glm::vec3 scale{ 1.0f };

    glm::mat4 localMatrix{ 1.0f };
    glm::mat4 globalMatrix{ 1.0f };

    int glMeshIndex = -1; 
    int skinIndex = -1;  

    AABB localAABB;
    AABB worldAABB;
};

struct Skin {
    std::vector<int> joints;       
    std::vector<glm::mat4> inverseBind;  
    std::vector<glm::mat4> jointMatrices; 
};

struct AnimationSampler {
    std::vector<float> times;     
    std::vector<glm::vec4> values; 
};

struct AnimationChannel {
    enum class Path {
        Translation,
        Rotation,
        Scale
    };

    int samplerIndex;
    int nodeIndex;
    Path path;
};

struct Animation {
    std::vector<AnimationSampler> samplers;
    std::vector<AnimationChannel> channels;
    float duration = 0.0f;
};

class tinyModel {
public:
    tinygltf::Model model;
    std::vector<GLTFMaterialGPU> gpuMaterials;
    std::vector<Mesh> glMeshes;
    std::vector<Node> nodes;
    int materialOffset;

    tinyModel(const std::string& path) {
        load_model(path);

        int sceneIndex = model.defaultScene >= 0 ? model.defaultScene : 0;
        const tinygltf::Scene& scene = model.scenes[sceneIndex];
   
        gpuMaterials.resize(model.materials.size());
        for (size_t i = 0; i < model.materials.size(); ++i) {
            gpuMaterials[i] = ParseGLTFMaterial(model, model.materials[i]);
        }      
        loadMeshes();
        loadNodes();   
        updateNodeTransforms(nodes);
    }

    void updateNodeTransforms(std::vector<Node>& nodes) {
        for (Node& n : nodes) {
            if (n.parent == -1)
                updateNodeGlobalRecursive(n, nodes);
        }
    }

private:
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    std::vector<int> meshNodeToGLMesh;

    void load_model(const std::string& path) {
        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
        if (!warn.empty()) std::cout << "Warn: " << warn << std::endl;
        if (!err.empty())  std::cerr << "Err: " << err << std::endl;
        if (!ret) throw std::runtime_error("Failed to load glTF");
    }

    void loadNodes()
    {
        nodes.resize(model.nodes.size());

        for (size_t i = 0; i < model.nodes.size(); ++i) {
            const tinygltf::Node& src = model.nodes[i];
            Node& dst = nodes[i];

            dst.children = src.children;
            dst.parent = -1;
            if (src.mesh >= 0) {
                dst.glMeshIndex = meshNodeToGLMesh[src.mesh];
                //dst.localAABB = combinePrimitivesAABB(src.mesh);
            }
            else {
                dst.glMeshIndex = -1;
            }
            if (!src.matrix.empty()) {
                glm::mat4 m = glm::make_mat4(src.matrix.data());
                dst.translation = glm::vec3(m[3][0], m[3][1], m[3][2]);
                dst.scale.x = glm::length(glm::vec3(m[0][0], m[0][1], m[0][2]));
                dst.scale.y = glm::length(glm::vec3(m[1][0], m[1][1], m[1][2]));
                dst.scale.z = glm::length(glm::vec3(m[2][0], m[2][1], m[2][2]));
                glm::mat3 rotMat;
                rotMat[0] = glm::vec3(m[0][0], m[0][1], m[0][2]) / dst.scale.x;
                rotMat[1] = glm::vec3(m[1][0], m[1][1], m[1][2]) / dst.scale.y;
                rotMat[2] = glm::vec3(m[2][0], m[2][1], m[2][2]) / dst.scale.z;
                dst.rotation = glm::quat_cast(rotMat);
            }
            else {
                dst.translation = src.translation.empty() ? glm::vec3(0.0f) : stdVec3ToGlm(src.translation);
                dst.rotation = src.rotation.empty() ? glm::quat(1, 0, 0, 0)
                    : glm::quat(src.rotation[3], src.rotation[0], src.rotation[1], src.rotation[2]);
                dst.scale = src.scale.empty() ? glm::vec3(1.0f) : stdVec3ToGlm(src.scale);
            }
            dst.skinIndex = src.skin;
        }
        for (size_t i = 0; i < nodes.size(); ++i)
            for (int c : nodes[i].children)
                nodes[c].parent = static_cast<int>(i);
    }

    void loadMeshes()
    {
        meshNodeToGLMesh.resize(model.meshes.size());
        int glMeshCounter = 0;
        for (size_t i = 0; i < model.meshes.size(); ++i) {
            const tinygltf::Mesh& mesh = model.meshes[i];
            meshNodeToGLMesh[i] = glMeshCounter; 
            for (const auto& prim : mesh.primitives) {
                createGlMesh(model, prim);
                glMeshCounter++;
            }
        }
    }

    void updateNodeGlobalRecursive(Node& node, std::vector<Node>& nodes) {
        node.localMatrix = glm::translate(glm::mat4(1.0f), node.translation) * glm::mat4_cast(node.rotation) * glm::scale(glm::mat4(1.0f), node.scale);
        if (node.parent >= 0)
            node.globalMatrix = nodes[node.parent].globalMatrix * node.localMatrix;
        else
            node.globalMatrix = node.localMatrix;
        for (int childIndex : node.children)
            updateNodeGlobalRecursive(nodes[childIndex], nodes);
    }

    void createGlMesh( const tinygltf::Model& model, const tinygltf::Primitive& primitive) {
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
        mesh.localAABB = localAABB;
        
        if (primitive.material >= 0) {
            mesh.materialIndex = primitive.material;
        }
        else {
            mesh.materialIndex = -1;
        }
        glMeshes.push_back(std::move(mesh));
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
