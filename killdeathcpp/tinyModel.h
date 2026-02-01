#pragma once

#define ANIM_DEBUG 0
#if ANIM_DEBUG
#define ANIM_LOG(x) std::cout << x << std::endl
#else
#define ANIM_LOG(x)
#endif

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

    AABB worldAABB;
};

struct Skin {
    int skeletonRoot = -1;   
    glm::mat4 invRoot;
    std::vector<int> joints;          
    std::vector<glm::mat4> inverseBind;
    std::vector<glm::mat4> jointMatrices;
};

struct AnimationSampler {
    size_t lastIndex = 0;
    std::vector<float> inputs;  
    std::vector<glm::vec4> outputs;     
    std::string interpolation;          
};

struct AnimationChannel {
    size_t nodeIndex;  
    int samplerIndex;
    enum class Path { Translation, Rotation, Scale } path;
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
    std::vector<Skin> skins;
    std::vector<ModelHitbox> hitboxes;
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
        updateNodeTransforms();
        loadAnimations();
        loadSkins();
        createModelHitBoxes();
    }

    void updateNodeTransforms() {
        for (Node& n : nodes) {
            if (n.parent == -1)
                updateNodeGlobalRecursive(n, nodes);
        }
    }


    void updateAnimation(float deltaTime)
    {
        Animation& anim = animations[activeAnimation];
        float prevTime = animationTime;
        animationTime = fmod(animationTime + deltaTime, anim.duration);

        for (const AnimationChannel& channel : anim.channels) {
            Node& node = nodes[channel.nodeIndex];
            AnimationSampler& sampler = anim.samplers[channel.samplerIndex];

            if (animationTime < sampler.inputs[sampler.lastIndex])sampler.lastIndex = 0;
            size_t i = sampler.lastIndex;
            while (i + 1 < sampler.inputs.size() && sampler.inputs[i + 1] <= animationTime) i++;
            if (i + 1 >= sampler.inputs.size()) i = 0;

            size_t j = i + 1;
            float t0 = sampler.inputs[i];
            float t1 = sampler.inputs[j];
            float alpha = (t1 > t0) ? (animationTime - t0) / (t1 - t0) : 0.0f;

            if (channel.path == AnimationChannel::Path::Translation) {
                glm::vec3 a = glm::vec3(sampler.outputs[i]);
                glm::vec3 b = glm::vec3(sampler.outputs[j]);  
                node.translation = glm::mix(a, b, alpha);
            }
            else if (channel.path == AnimationChannel::Path::Rotation) {
                glm::vec4 va = sampler.outputs[i];
                glm::vec4 vb = sampler.outputs[j];
                glm::quat a(va.w, va.x, va.y, va.z);
                glm::quat b(vb.w, vb.x, vb.y, vb.z);
                node.rotation = glm::slerp(a, b, alpha);
            }
            else if (channel.path == AnimationChannel::Path::Scale) {
                glm::vec3 a = glm::vec3(sampler.outputs[i]);
                glm::vec3 b = glm::vec3(sampler.outputs[j]);
                node.scale = glm::mix(a, b, alpha);
            }
        }
    }


    void updateSkins()
    {
        if (skins.empty())
            return;
        for (Skin& skin : skins) {
            skin.jointMatrices.resize(skin.joints.size());
            for (size_t i = 0; i < skin.joints.size(); ++i) {
                int jointNodeIndex = skin.joints[i];
                skin.jointMatrices[i] = nodes[jointNodeIndex].globalMatrix * skin.inverseBind[i];             
            }
        }
    } 

private:
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    std::vector<int> meshNodeToGLMesh;
    std::vector<Animation> animations;
    int activeAnimation = 0;
    float animationTime = 0.0f;


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
            dst.skinIndex = src.skin;

            if (src.mesh >= 0)
                dst.glMeshIndex = meshNodeToGLMesh[src.mesh];
            else
                dst.glMeshIndex = -1;

            if (!src.matrix.empty()) {
                //warning hardcoded unitscale!!!
                float unitScale = 0.01f;
                glm::mat4 M = glm::make_mat4(src.matrix.data());
                glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(unitScale));
                dst.localMatrix = S * M;
                dst.translation = glm::vec3(dst.localMatrix[3]);
                dst.rotation = glm::quat_cast(glm::mat3(dst.localMatrix));               
                dst.scale = glm::vec3(1.0f);
                
            }
            else {
                dst.translation = src.translation.empty() ? glm::vec3(0.0f)  : stdVec3ToGlm(src.translation);
                dst.rotation = src.rotation.empty() ? glm::quat(1, 0, 0, 0) : glm::quat(src.rotation[3], src.rotation[0], src.rotation[1], src.rotation[2]);
                dst.scale = src.scale.empty() ? glm::vec3(1.0f) : stdVec3ToGlm(src.scale);                
            }
            
        }
        for (size_t i = 0; i < nodes.size(); ++i)
            for (int c : nodes[i].children)
                nodes[c].parent = int(i);
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

    void loadSkins()
    {
        for (const tinygltf::Skin& src : model.skins) {
            Skin skin;
            skin.skeletonRoot = src.skeleton;
            skin.joints = src.joints;
            if (src.inverseBindMatrices >= 0) {
                ReadAccessor(model,model.accessors[src.inverseBindMatrices],skin.inverseBind);
            }
            else {
                skin.inverseBind.resize(skin.joints.size(), glm::mat4(1.0f));
            }          
            skins.push_back(skin);
        }
    }

    void createModelHitBoxes()
    {
        for (const Skin& skin : skins) {
            for (size_t i = 0; i + 1 < skin.joints.size(); ++i) {
                int jointA = skin.joints[i];
                int jointB = skin.joints[i + 1];
                glm::vec3 pA = glm::vec3(nodes[jointA].globalMatrix[3]);
                glm::vec3 pB = glm::vec3(nodes[jointB].globalMatrix[3]);
                float boneLength = glm::length(pB - pA);
                ModelHitbox hb;
                hb.shape = HitboxShape::Capsule;
                hb.node = jointA;
                hb.halfHeight = boneLength * 0.5f;
                hb.radius = boneLength * 0.15f;
                hb.localOffset = glm::mat4(1.0f);
                hitboxes.push_back(hb);
            }
        }
    }

    void updateNodeGlobalRecursive(Node& node, std::vector<Node>& nodes) {
        node.localMatrix = glm::translate(glm::mat4(1.0f), node.translation) * glm::mat4_cast(node.rotation) * glm::scale(glm::mat4(1.0f), node.scale);
        if (node.parent >= 0) {
            node.globalMatrix = nodes[node.parent].globalMatrix * node.localMatrix;
        }
        else
            node.globalMatrix = node.localMatrix;

        if (node.glMeshIndex >= 0 && model.skins.size() == 0) {
            const AABB& local = glMeshes[node.glMeshIndex].localAABB;
            node.worldAABB = computeWorldAABB(local, node.globalMatrix);
        }

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

    void loadAnimations() {
        for (const tinygltf::Animation& srcAnim : model.animations) {
            Animation anim;
            for (const auto& srcSampler : srcAnim.samplers) {
                AnimationSampler sampler;
                ReadAccessor(model, model.accessors[srcSampler.input], sampler.inputs);
                ReadAccessor(model, model.accessors[srcSampler.output], sampler.outputs);

                sampler.interpolation = srcSampler.interpolation;
                anim.samplers.push_back(sampler);
                anim.duration = std::max(anim.duration, sampler.inputs.back());
            }
            for (const auto& srcChannel : srcAnim.channels) {
                AnimationChannel channel;
                channel.nodeIndex = srcChannel.target_node;
                channel.samplerIndex = srcChannel.sampler;

                if (srcChannel.target_path == "translation")
                    channel.path = AnimationChannel::Path::Translation;
                else if (srcChannel.target_path == "rotation")
                    channel.path = AnimationChannel::Path::Rotation;
                else if (srcChannel.target_path == "scale")
                    channel.path = AnimationChannel::Path::Scale;

                anim.channels.push_back(channel);
            }
            animations.push_back(anim);
        }
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
