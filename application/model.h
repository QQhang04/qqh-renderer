//
// Created by qihang.qian on 10/24/2025.
//

#ifndef GRAPHIC_MODEL_H
#define GRAPHIC_MODEL_H


#include "image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "../math/math.h"

class LightShader;

struct Vertex {
    math::vec3f position;
    math::vec3f normal;
    math::vec2f texCoords;
};

class Model;

class Mesh {
public:
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int> indices,
        uint32_t diffuseTexture,
        const math::Mat4f& localMatrix
    );

    ~Mesh();

    void addChild(Mesh* mesh)
    {
        mChildren.push_back(mesh);
    }

    void draw(const math::Mat4f& preMatrix, LightShader* shader);

private:
    friend class Model;
    uint32_t mVao{ 0 }; // 为0表示只作为节点，没有实际数据
    uint32_t mVbo{ 0 };
    uint32_t mEbo{ 0 };
    uint32_t mTexture{ 0 };
    uint32_t mIndicesCount{ 0 };

    math::Mat4f mLocalMatrix; // 相对于父节点的变换

    std::vector<Mesh*> mChildren;
};

class Model {
public:
    Model();
    ~Model();

    void read(const std::string& path);

    void draw(LightShader* shader);

    void setModelMatrix(const math::Mat4f& m);

private:
    void processNode(Mesh* parent, aiNode* ainode, const aiScene* scene);

    Mesh* processMesh(aiMesh* aimesh, const aiScene* scene);

    uint32_t processTexture(
        const aiMaterial* material,
        const aiTextureType& type,
        const aiScene* scene
    );

    uint32_t createTexture(Image* image);

    static math::Mat4f getMat4f(aiMatrix4x4 value);

private:
    std::string mRootPath{};
    Mesh* mRootMesh{ nullptr };
    std::vector<Mesh*> mMeshes; // 便于析构时一起释放资源

    //image path and textureID
    std::map<std::string, uint32_t> mTextureCache; // 多个mesh可能对应同一个纹理
};
#endif //GRAPHIC_MODEL_H