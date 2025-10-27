#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"
#include "ModelMesh.h"
#include <filesystem>
#include <glm/gtx/quaternion.hpp>

struct BoneInfo {
    glm::mat4 offset;
    glm::mat4 finalTransform;
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
AABB computeUnionHitbox(const std::vector<ModelMesh>& meshes);
glm::mat4 AssimpToGlmMat4(const aiMatrix4x4& from);
void setVertexBoneData(Vertex& vertex, int boneID, float weight);
glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
glm::vec3 InterpolateScaling(float animationTime, const aiNodeAnim* nodeAnim);
glm::vec3 InterpolatePosition(float animationTime, const aiNodeAnim* nodeAnim);
const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);

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
    std::unordered_map<std::string, int> m_BoneMapping;
    std::vector<BoneInfo> m_BoneInfo;
    int m_BoneCount = 0;
    glm::mat4 globalInverseTransform;
    const aiScene* scene = nullptr;
	bool loading = true;
    bool hasBones = false;

    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
        coarse_AABB = computeUnionHitbox(meshes);
       
    }
    void Draw(Shader& shader)
    {

        orientation.updateShader(&shader);
        SetBoneTransforms(shader);
        //std::cout << directory << "model directory \n";
        //shader.PrintDebugUniforms();
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
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
    void loadModel(string const& path)
    {
        scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        globalInverseTransform = glm::inverse(AssimpToGlmMat4(scene->mRootNode->mTransformation));
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);

        hasBones = (scene->mNumAnimations > 0);
        for (unsigned int i = 0; i < scene->mNumMeshes && !hasBones; ++i) {
            if (scene->mMeshes[i]->mNumBones > 0)
                hasBones = true;
        }

        if (hasBones) InitializeBoneTransforms();

        loading = false;

    }

    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform = glm::mat4(1.0f))
    {
        std::cout << "Visiting node: " << node->mName.C_Str() << std::endl;
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, parentTransform));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, parentTransform);
        }
    }

    ModelMesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& transform)
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
                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
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
        if (hasBones)extractBoneWeights(vertices, mesh, scene);
        return ModelMesh(vertices, indices, textures);
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

    void extractBoneWeights(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
        std::cout << "Extracting bone weights for mesh: " << mesh->mName.C_Str() << std::endl;
        for (unsigned int i = 0; i < mesh->mNumBones; i++) {
            aiBone* bone = mesh->mBones[i];
            std::string boneName(bone->mName.data);
            int boneIndex = 0;
            if (m_BoneMapping.find(boneName) == m_BoneMapping.end()) {
                boneIndex = m_BoneCount;
                m_BoneMapping[boneName] = m_BoneCount;
                BoneInfo bi;
                bi.offset = AssimpToGlmMat4(bone->mOffsetMatrix);
                m_BoneInfo.push_back(bi);
                m_BoneCount++;
                glm::mat4 offsetMat = bi.offset;
                for (int r = 0; r < 4; r++) {
                    std::cout << "      ";
                    for (int c = 0; c < 4; c++) {
                        std::cout << offsetMat[c][r] << " ";
                    }
                    std::cout << std::endl;
                }
            }
            else {
                boneIndex = m_BoneMapping[boneName];
            }

            for (unsigned int j = 0; j < bone->mNumWeights; j++) {
                int vertexId = bone->mWeights[j].mVertexId;
                float weight = bone->mWeights[j].mWeight;
                setVertexBoneData(vertices[vertexId], boneIndex, weight);
            }
        }
    }

    void ReadNodeHierarchy(const aiNode* node, const glm::mat4& parentTransform)
    {
        glm::mat4 nodeTransform = AssimpToGlmMat4(node->mTransformation);
        glm::mat4 globalTransform = parentTransform * nodeTransform;
        std::string nodeName(node->mName.data);
        if (m_BoneMapping.find(nodeName) != m_BoneMapping.end()) {
            int boneIndex = m_BoneMapping[nodeName];
            m_BoneInfo[boneIndex].finalTransform =
                globalInverseTransform * globalTransform * m_BoneInfo[boneIndex].offset;
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ReadNodeHierarchy(node->mChildren[i], globalTransform);
        }
    }

    void SetBoneTransforms(Shader& shader) {
        shader.use();
        for (int i = 0; i < m_BoneCount; ++i) {
            std::string name = "bones[" + std::to_string(i) + "]";
            GLint loc = glGetUniformLocation(shader.ID, name.c_str());
            if (loc != -1) {
                glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m_BoneInfo[i].finalTransform));
            }
        }
    }

    void InitializeBoneTransforms() {
        for (auto& bone : m_BoneInfo) {
            bone.finalTransform = glm::mat4(1.0f);
        }
        if (scene && scene->mRootNode) {
            ReadNodeHierarchy(scene->mRootNode, glm::mat4(1.0f));
        }
    }

    void BoneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms) {
        if (!scene || scene->mNumAnimations == 0) return;
        const aiAnimation* animation = scene->mAnimations[0];
        float ticksPerSecond = (float)(animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0);
        float timeInTicks = timeInSeconds * ticksPerSecond;
        float animationTime = fmod(timeInTicks, (float)animation->mDuration);
        glm::mat4 identity = glm::mat4(1.0f);
        ReadNodeHierarchy(animationTime, scene->mRootNode, identity, animation);
        transforms.resize(m_BoneCount);
        for (int i = 0; i < m_BoneCount; i++) {
            transforms[i] = m_BoneInfo[i].finalTransform;
        }
    }

    void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform, const aiAnimation* animation) {
        std::string nodeName(node->mName.data);
        glm::mat4 nodeTransform = AssimpToGlmMat4(node->mTransformation);
        const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);
        if (nodeAnim) {
            glm::vec3 scaling = InterpolateScaling(animationTime, nodeAnim);
            glm::mat4 scalingM = glm::scale(glm::mat4(1.0f), scaling);
            glm::quat rotationQ = InterpolateRotation(animationTime, nodeAnim);
            glm::mat4 rotationM = glm::toMat4(rotationQ);
            glm::vec3 translation = InterpolatePosition(animationTime, nodeAnim);
            glm::mat4 translationM = glm::translate(glm::mat4(1.0f), translation);
            nodeTransform = translationM * rotationM * scalingM;
        }
        glm::mat4 globalTransform = parentTransform * nodeTransform;
        if (m_BoneMapping.find(nodeName) != m_BoneMapping.end()) {
            int boneIndex = m_BoneMapping[nodeName];
            m_BoneInfo[boneIndex].finalTransform =
                globalTransform * m_BoneInfo[boneIndex].offset;
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransform, animation);
        }
    }

};



glm::vec3 InterpolatePosition(float animationTime, const aiNodeAnim* nodeAnim) {
    if (nodeAnim->mNumPositionKeys == 1)
        return glm::vec3(nodeAnim->mPositionKeys[0].mValue.x, nodeAnim->mPositionKeys[0].mValue.y, nodeAnim->mPositionKeys[0].mValue.z);
    for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++) {
        if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime) {
            float t1 = (float)nodeAnim->mPositionKeys[i].mTime;
            float t2 = (float)nodeAnim->mPositionKeys[i + 1].mTime;
            float factor = (animationTime - t1) / (t2 - t1);
            aiVector3D start = nodeAnim->mPositionKeys[i].mValue;
            aiVector3D end = nodeAnim->mPositionKeys[i + 1].mValue;
            aiVector3D delta = end - start;
            aiVector3D result = start + factor * delta;
            return glm::vec3(result.x, result.y, result.z);
        }
    }
    return glm::vec3(0.0f);
}

glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim) {
    if (nodeAnim->mNumRotationKeys == 1) {
        aiQuaternion q = nodeAnim->mRotationKeys[0].mValue;
        return glm::quat(q.w, q.x, q.y, q.z);
    }
    for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++) {
        if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime) {
            float t1 = (float)nodeAnim->mRotationKeys[i].mTime;
            float t2 = (float)nodeAnim->mRotationKeys[i + 1].mTime;
            float factor = (animationTime - t1) / (t2 - t1);
            aiQuaternion q1 = nodeAnim->mRotationKeys[i].mValue;
            aiQuaternion q2 = nodeAnim->mRotationKeys[i + 1].mValue;
            aiQuaternion blended;
            aiQuaternion::Interpolate(blended, q1, q2, factor);
            blended = blended.Normalize();
            return glm::quat(blended.w, blended.x, blended.y, blended.z);
        }
    }
    return glm::quat(1, 0, 0, 0);
}

glm::vec3 InterpolateScaling(float animationTime, const aiNodeAnim* nodeAnim) {
    if (nodeAnim->mNumScalingKeys == 1)
        return glm::vec3(nodeAnim->mScalingKeys[0].mValue.x,
            nodeAnim->mScalingKeys[0].mValue.y,
            nodeAnim->mScalingKeys[0].mValue.z);
    for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
        if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
            float t1 = (float)nodeAnim->mScalingKeys[i].mTime;
            float t2 = (float)nodeAnim->mScalingKeys[i + 1].mTime;
            float factor = (animationTime - t1) / (t2 - t1);
            aiVector3D start = nodeAnim->mScalingKeys[i].mValue;
            aiVector3D end = nodeAnim->mScalingKeys[i + 1].mValue;
            aiVector3D delta = end - start;
            aiVector3D result = start + factor * delta;
            return glm::vec3(result.x, result.y, result.z);
        }
    }
    return glm::vec3(1.0f);
}



const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName) {
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        const aiNodeAnim* nodeAnim = animation->mChannels[i];
        if (std::string(nodeAnim->mNodeName.data) == nodeName)
            return nodeAnim;
    }
    return nullptr;
}

void setVertexBoneData(Vertex& vertex, int boneID, float weight) {
    for (int i = 0; i < 4; i++) {
        if (vertex.m_BoneIDs[i] < 0) {
            vertex.m_BoneIDs[i] = boneID;
            vertex.m_Weights[i] = weight;
			printf("Assigned boneID %d with weight %f to vertex\n", boneID, weight);
        }
    }
}

glm::mat4 AssimpToGlmMat4(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

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