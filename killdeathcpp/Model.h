#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"
#include "ModelMesh.h"
#include <filesystem>
#include <glm/gtx/quaternion.hpp>

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
AABB computeUnionHitbox(const std::vector<ModelMesh>& meshes);
glm::mat4 aiMatrixToGlm(const aiMatrix4x4& from);

struct BoneInfo
{
    int id;
    glm::mat4 offset;
};

void printAnimationData(const aiScene* scene)
{
    std::cout << "Scene has " << scene->mNumAnimations << " animations\n";

    for (unsigned int a = 0; a < scene->mNumAnimations; a++)
    {
        const aiAnimation* anim = scene->mAnimations[a];
        std::cout << "Animation " << a << ": " << anim->mName.C_Str() << "\n";
        std::cout << "  Duration: " << anim->mDuration
            << " ticks, TicksPerSecond: " << anim->mTicksPerSecond << "\n";
        std::cout << "  Channels: " << anim->mNumChannels << "\n";

        for (unsigned int c = 0; c < anim->mNumChannels; c++)
        {
            const aiNodeAnim* channel = anim->mChannels[c];
            std::cout << "    Node: " << channel->mNodeName.C_Str() << "\n";
            std::cout << "      Positions: " << channel->mNumPositionKeys << "\n";
            std::cout << "      Rotations: " << channel->mNumRotationKeys << "\n";
            std::cout << "      Scalings:  " << channel->mNumScalingKeys << "\n";

            // Example: print first key of each type
            if (channel->mNumPositionKeys > 0) {
                aiVector3D pos = channel->mPositionKeys[0].mValue;
                std::cout << "        First pos: ("
                    << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
            }
            if (channel->mNumRotationKeys > 0) {
                aiQuaternion rot = channel->mRotationKeys[0].mValue;
                std::cout << "        First rot (quat): ("
                    << rot.x << ", " << rot.y << ", " << rot.z << ", " << rot.w << ")\n";
            }
            if (channel->mNumScalingKeys > 0) {
                aiVector3D scl = channel->mScalingKeys[0].mValue;
                std::cout << "        First scale: ("
                    << scl.x << ", " << scl.y << ", " << scl.z << ")\n";
            }
        }
    }
}



class Model
{
public:
    vector<Texture> textures_loaded;
    vector<ModelMesh>    meshes;
    Assimp::Importer importer;
    string directory;
    bool gammaCorrection;
    ObjectOrientation orientation;
    AABB coarse_AABB;


    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
        coarse_AABB = computeUnionHitbox(meshes);
    }
    void Draw(Shader& shader)
    {

        orientation.updateShader(&shader);
        //std::cout << directory << "model directory \n";
        //shader.PrintDebugUniforms();
        for (unsigned int i = 0; i < meshes.size(); i++) {
            shader.use();
            shader.setMat4("model", orientation.modelMatrix * meshes[i].modelMatrix);
            meshes[i].Draw(shader);
        }
    }



    void rotate(const glm::vec3& angleDelta) {
        orientation.rotate(angleDelta);
        updateMeshHitboxes();
        coarse_AABB = computeUnionHitbox(meshes);
    }
    void changeSize(const glm::vec3& scaleFactor) {
        orientation.changeSize(scaleFactor);
        updateMeshHitboxes();
        coarse_AABB = computeUnionHitbox(meshes);
    }
    void movePos(const glm::vec3& delta) {
        orientation.movePos(delta);
        updateMeshHitboxes();
        coarse_AABB = computeUnionHitbox(meshes);
    }
    void updateMeshHitboxes() {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].hitbox.updateModelMatrix(orientation.modelMatrix);
    }

private:
    std::unordered_map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }
        void SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    void loadModel(string const& path)
    {
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        printAnimationData(scene);
        glm::mat4 globalScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
        processNode(scene->mRootNode, scene, globalScale);
    }

    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform)
    {
        glm::mat4 nodeTransform = aiMatrixToGlm(node->mTransformation);
        glm::mat4 globalTransform = parentTransform * nodeTransform;
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, globalTransform));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, globalTransform);
        }
    }

    ModelMesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& parentTransform)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            if (mesh->HasNormals())
            {
                glm::mat3 normalMatrix = glm::mat3(1.0f);
                glm::vec3 norm = normalMatrix * glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                vertex.normal = glm::normalize(norm);
            }

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;

                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        ModelMesh model = ModelMesh(vertices, indices, textures);
        model.modelMatrix = parentTransform;
        return model;
    }

    void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_WEIGHTS; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }

    void ExtractBoneWeightForVertices(std::vector& vertices, aiMesh* mesh, const aiScene* scene)
    {
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = m_BoneCounter;
                newBoneInfo.offset = aiMatrixToGlm(mesh->mBones[boneIndex]->mOffsetMatrix);
                m_BoneInfoMap[boneName] = newBoneInfo;
                boneID = m_BoneCounter;
                m_BoneCounter++;
            }
            else
            {
                boneID = m_BoneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
};


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{

    filesystem::path p(directory);
    p.replace_filename(path);
    string filename = p.string();

    std::cout << filename << "\n";

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}



AABB computeUnionHitbox(const std::vector<ModelMesh>& meshes) {
    AABB result;
    result.min = glm::vec3(FLT_MAX);
    result.max = glm::vec3(-FLT_MAX);
    for (const ModelMesh mesh : meshes) {
        const HitBox& hb = mesh.hitbox;
        result.min.x = std::min(result.min.x, hb.min.x);
        result.min.y = std::min(result.min.y, hb.min.y);
        result.min.z = std::min(result.min.z, hb.min.z);
        result.max.x = std::max(result.max.x, hb.max.x);
        result.max.y = std::max(result.max.y, hb.max.y);
        result.max.z = std::max(result.max.z, hb.max.z);
    }
    return result;
}

glm::mat4 aiMatrixToGlm(const aiMatrix4x4& from)
{
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

bool CheckModelCollision(Model& model, Camera& camera) {
    bool collided = false;
    for (ModelMesh mesh : model.meshes) {
        if (point_Box_Colission(mesh.hitbox, camera.position, camera.movement)) {
            collided = true;
        }
    }
    return collided;
}

bool checkGroundedOnMeshes(Model& model, Camera& camera) {
    bool grounded = false;
    for (ModelMesh mesh : model.meshes) {
        grounded |= camera.isGrounded(mesh.hitbox);
    }
    return grounded;
}



#endif