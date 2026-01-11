#ifndef MODELHELPERS_H
#define MODELHELPERS_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/postprocess.h>
#include <tiny_gltf/tiny_gltf.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <glad/glad.h>
#include "Mesh.h"


struct GLTFMaterialGPU {
    glm::vec4 baseColorFactor{ 1.0f };
    Texture baseColorTex;

    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    Texture metallicRoughnessTex;

    float normalScale = 1.0f;
    Texture normalTex;

    Texture occlusionTex;

    glm::vec3 emissiveFactor{ 0.0f };
    Texture emissiveTex;

    bool doubleSided = false;
};

inline void PrintMat4(const glm::mat4& m) {
    for (int i = 0; i < 4; i++) {
        std::cout << m[i][0] << " "
            << m[i][1] << " "
            << m[i][2] << " "
            << m[i][3] << std::endl;
    }
}

inline void printVec3(const glm::vec3& v) {
    std::cout << "vector x: " << v.x
        << " vector y: " << v.y
        << " vector z: " << v.z << std::endl;
}

inline void printVec4(const glm::vec3& v) {
    std::cout << "vector x: " << v.x
        << " vector y: " << v.y
        << " vector z: " << v.z
        << " vector b: " << v.b << std::endl;
}


inline glm::vec3 aiVectorToGlm(const aiVector3D& v) {
    return glm::vec3(v.x, v.y, v.z);
}

inline glm::mat4 aiMatrixToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

template<typename T>
inline glm::vec3 stdVec3ToGlm(const std::vector<T>& v) {
    if (v.size() != 3) {
        throw std::runtime_error("stdVec3ToGlm: vector size != 3");
    }
    return glm::vec3(v[0], v[1], v[2]);
}

template<typename T>
inline glm::vec4 stdVec4ToGlm(const std::vector<T>& v) {
    if (v.size() != 4) {
        throw std::runtime_error("stdVec4ToGlm: vector size != 4");
    }
    return glm::vec4(v[0], v[1], v[2], v[3]);
}

// ================= TinyGLTF Helpers =================

inline size_t ComponentSize(int componentType) {
    switch (componentType) {
    case TINYGLTF_COMPONENT_TYPE_BYTE:           return 1;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  return 1;
    case TINYGLTF_COMPONENT_TYPE_SHORT:          return 2;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: return 2;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:   return 4;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:          return 4;
    default: throw std::runtime_error("Invalid component type");
    }
}

inline Texture TextureFromGLTFImage(const tinygltf::Image& img, bool srgb)
{   
    if (img.image.empty()) {
        std::cerr << "[TextureFromGLTFImage] Image data is empty!\n";
        return Texture{ 0 };
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    GLenum internalFormat = GL_RGBA8;
    GLenum format = GL_RGBA;

    switch (img.component) {
    case 1: internalFormat = GL_R8; format = GL_RED; break;
    case 2: internalFormat = GL_RG8; format = GL_RG; break;
    case 3: internalFormat = srgb ? GL_SRGB8 : GL_RGB8; format = GL_RGB; break;
    case 4: internalFormat = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8; format = GL_RGBA; break;
    default:
        std::cerr << "[TextureFromGLTFImage] Unsupported component count: " << img.component << "\n";
        return Texture{ 0 };
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, img.width, img.height, 0, format, GL_UNSIGNED_BYTE, img.image.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
;
    Texture tex;
    tex.id = textureID;
    return tex;
}

inline void ApplyGLTFSampler(
    GLuint textureID,
    const tinygltf::Model& model,
    const tinygltf::Texture& texture)
{
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint wrapS = GL_REPEAT;
    GLint wrapT = GL_REPEAT;
    GLint minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLint magFilter = GL_LINEAR;

    if (texture.sampler >= 0) {
        const tinygltf::Sampler& sampler = model.samplers[texture.sampler];
        if (sampler.wrapS != -1) wrapS = sampler.wrapS;
        if (sampler.wrapT != -1) wrapT = sampler.wrapT;
        if (sampler.minFilter != -1) minFilter = sampler.minFilter;
        if (sampler.magFilter != -1) magFilter = sampler.magFilter;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}



inline size_t TypeCount(int type) {
    switch (type) {
    case TINYGLTF_TYPE_SCALAR: return 1;
    case TINYGLTF_TYPE_VEC2:   return 2;
    case TINYGLTF_TYPE_VEC3:   return 3;
    case TINYGLTF_TYPE_VEC4:   return 4;
    case TINYGLTF_TYPE_MAT4:   return 16;
    default: throw std::runtime_error("Invalid type");
    }
}

template<typename T>
inline void ReadAccessor(const tinygltf::Model& model,
    const tinygltf::Accessor& accessor,
    std::vector<T>& out)
{
    const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer = model.buffers[view.buffer];
    size_t componentCount = TypeCount(accessor.type);
    size_t componentSize = ComponentSize(accessor.componentType);
    size_t stride = view.byteStride ? view.byteStride : componentCount * componentSize;
    const unsigned char* src = buffer.data.data() + view.byteOffset + accessor.byteOffset;

    out.resize(accessor.count);

    for (size_t i = 0; i < accessor.count; ++i) {
        T value{};
        for (size_t c = 0; c < componentCount; ++c) {
            const unsigned char* ptr = src + i * stride + c * componentSize;
            float f = 0.0f;

            switch (accessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:          f = *reinterpret_cast<const float*>(ptr); break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: f = *ptr; break;
            case TINYGLTF_COMPONENT_TYPE_BYTE:          f = *reinterpret_cast<const int8_t*>(ptr); break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:f = *reinterpret_cast<const uint16_t*>(ptr); break;
            case TINYGLTF_COMPONENT_TYPE_SHORT:         f = *reinterpret_cast<const int16_t*>(ptr); break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:  f = *reinterpret_cast<const uint32_t*>(ptr); break;
            default: throw std::runtime_error("Unsupported component type");
            }

            if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec2>) {
                (&value[0])[c] = f;
            }
            else if constexpr (std::is_same_v<T, float>) {
                if (c == 0) value = f;
            }
            else if constexpr (std::is_same_v<T, glm::uvec4>) {
                (&value[0])[c] = static_cast<uint32_t>(f);
            }
            else if constexpr (std::is_integral_v<T>) {
                if (c == 0) value = static_cast<T>(f);
            }
            else {
                static_assert(!sizeof(T*), "Unhandled type in ReadAccessor");
            }
        }
        out[i] = value;
    }
}



inline void DebugPrintVertex(const Vertex& v, size_t index = 0)
{
    std::cout << "Vertex[" << index << "]\n";
    std::cout << "  Position:  ("
        << v.position.x << ", "
        << v.position.y << ", "
        << v.position.z << ")\n";

    std::cout << "  Normal:    ("
        << v.normal.x << ", "
        << v.normal.y << ", "
        << v.normal.z << ")\n";

    std::cout << "  TexCoords: ("
        << v.texCoords.x << ", "
        << v.texCoords.y << ")\n";

    std::cout << "  Tangent:   ("
        << v.Tangent.x << ", "
        << v.Tangent.y << ", "
        << v.Tangent.z << ", "
        << v.Tangent.w << ")\n";

    std::cout << "  Joints:    ("
        << v.joints.x << ", "
        << v.joints.y << ", "
        << v.joints.z << ", "
        << v.joints.w << ")\n";

    std::cout << "  Weights:   ("
        << v.weights.x << ", "
        << v.weights.y << ", "
        << v.weights.z << ", "
        << v.weights.w << ")\n";
}

inline void DebugPrintMaterial(
    const GLTFMaterialGPU& mat,
    int materialIndex = -1
) {
    std::cout << "==============================\n";
    std::cout << "GLTFMaterialGPU";
    if (materialIndex >= 0)
        std::cout << " [" << materialIndex << "]";
    std::cout << "\n";

    std::cout << "Base Color Factor: ("
        << mat.baseColorFactor.r << ", "
        << mat.baseColorFactor.g << ", "
        << mat.baseColorFactor.b << ", "
        << mat.baseColorFactor.a << ")\n";

    std::cout << "Metallic Factor:  " << mat.metallicFactor << "\n";
    std::cout << "Roughness Factor: " << mat.roughnessFactor << "\n";

    std::cout << "Normal Scale:     " << mat.normalScale << "\n";

    std::cout << "Emissive Factor: ("
        << mat.emissiveFactor.r << ", "
        << mat.emissiveFactor.g << ", "
        << mat.emissiveFactor.b << ")\n";

    std::cout << "Double Sided:     "
        << (mat.doubleSided ? "true" : "false") << "\n";

    std::cout << "\nTextures:\n";
    std::cout << "  BaseColor:          "
        << (mat.baseColorTex.id ? "YES" : "NO") << "\n";
    std::cout << "  MetallicRoughness:  "
        << (mat.metallicRoughnessTex.id ? "YES" : "NO") << "\n";
    std::cout << "  Normal:             "
        << (mat.normalTex.id ? "YES" : "NO") << "\n";
    std::cout << "  Occlusion:          "
        << (mat.occlusionTex.id ? "YES" : "NO") << "\n";
    std::cout << "  Emissive:           "
        << (mat.emissiveTex.id ? "YES" : "NO") << "\n";

    std::cout << "==============================\n";
}

#endif // MODELHELPERS_H
