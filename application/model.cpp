//
// Created by qihang.qian on 10/24/2025.
//

#include "model.h"
#include "../gpu/gpu.h"

Model::Model() = default;   // 如果暂时不需要初始化，直接默认
// 或者显式： mRootMesh(nullptr) {}

Model::~Model() {
    // 释放 mesh 树
    for (auto m : mMeshes) delete m;
    for (auto & iter : mTextureCache)
    {
        sgl->deleteTexture(iter.second);
    }
}

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int> indices,
    uint32_t diffuseTexture,
    const math::Mat4f& localMatrix) {
    //至少作为一个节点
    mLocalMatrix = localMatrix;
    mTexture = diffuseTexture; // 由外部生成 不需要管理生命周期

    if (vertices.empty() || indices.empty()) {
        return;
    }

    mVao = sgl->genVertexArray();
    mVbo = sgl->genBuffer();
    mEbo = sgl->genBuffer();

    // 使用交叉混合vbo保存顶点信息
    sgl->bindBuffer(ARRAY_BUFFER, mVbo);
    sgl->bufferData(ARRAY_BUFFER, vertices.size() * sizeof(Vertex), (void*)&vertices[0]);

    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, mEbo);
    sgl->bufferData(ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), (void*)&indices[0]);
    mIndicesCount = indices.size();

    sgl->bindVertexArray(mVao);
    sgl->vertexAttributePointer(0, 3, sizeof(Vertex), 0); // pos
    sgl->vertexAttributePointer(1, 3, sizeof(Vertex), sizeof(float) * 3); // normal
    sgl->vertexAttributePointer(2, 2, sizeof(Vertex), sizeof(float) * 6); // uv
    sgl->bindVertexArray(0);
}

void Model::draw(LightShader* shader) {
    mRootMesh->draw(math::Mat4f(), shader);
}

void Model::setModelMatrix(const math::Mat4f& m) {
    mRootMesh->mLocalMatrix = m;
}

void Mesh::draw(const math::Mat4f& preMatrix, LightShader* shader) {
    auto modelMatrix = preMatrix * mLocalMatrix;

    if (mVao) {
        shader->mModelMatrix = modelMatrix;
        if (mTexture) {
            shader->mDiffuseTexture = mTexture;
        }

        sgl->bindVertexArray(mVao);
        sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, mEbo);
        sgl->drawElement(DRAW_TRIANGLES, 0, mIndicesCount);
        sgl->bindVertexArray(0);
    }

    for (auto mesh : mChildren) {
        mesh->draw(modelMatrix, shader);
    }
}

Mesh::~Mesh() {
    if (mVao) {
        sgl->deleteVertexArray(mVao);
    }
    if (mVbo) {
        sgl->deleteBuffer(mVbo);
    }
    if (mEbo) {
        sgl->deleteBuffer(mEbo);
    }
}

void Model::read(const std::string& path) {
    //assets/model/bag/bag.jpg --> assets/model/bag
    std::size_t lastIndex = path.find_last_of("//");
    mRootPath = path.substr(0, lastIndex + 1);

    //开始进行读取
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Error:model read fail!" << std::endl;
        return ;
    }

    mRootMesh = new Mesh({}, {}, 0, math::Mat4f());
    processNode(mRootMesh, scene->mRootNode, scene);
}

void Model::processNode(Mesh* parent, aiNode* ainode, const aiScene* scene) {
    //创建新节点
    auto node = new Mesh({}, {}, 0, math::Mat4f());
    parent->addChild(node);

    node->mLocalMatrix = getMat4f(ainode->mTransformation);

    // 处理节点所有的网格（如果有的话），挂载到本node下
    for (unsigned int i = 0; i < ainode->mNumMeshes; i++)
    {
        aiMesh* aimesh = scene->mMeshes[ainode->mMeshes[i]];
        auto mesh = processMesh(aimesh, scene);
        mMeshes.push_back(mesh);
        node->addChild(mesh);
    }

    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < ainode->mNumChildren; i++)
    {
        processNode(node, ainode->mChildren[i], scene);
    }
}

Mesh* Model::processMesh(aiMesh* aimesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    uint32_t diffuseTexture = 0;

    for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
    {
        Vertex vertex;
        // 处理顶点位置、法线和纹理坐标
        math::vec3f vector;
        vector.x = aimesh->mVertices[i].x;
        vector.y = aimesh->mVertices[i].y;
        vector.z = aimesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = aimesh->mNormals[i].x;
        vector.y = aimesh->mNormals[i].y;
        vector.z = aimesh->mNormals[i].z;
        vertex.normal = vector;

        if (aimesh->mTextureCoords[0]) // 网格是否有纹理坐标?
        {
            math::vec2f vec;
            vec.x = aimesh->mTextureCoords[0][i].x;
            vec.y = aimesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        }
        else
            vertex.texCoords = math::vec2f(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // 处理索引
    for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
    {
        aiFace face = aimesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (aimesh->mMaterialIndex >= 0)
    {
        //取出材质
        aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
        diffuseTexture = processTexture(material, aiTextureType_DIFFUSE, scene);
    }

    //其上方已经拥有了parentNode，不需要自己的localMatrix
    return new Mesh(vertices, indices, diffuseTexture, math::Mat4f());
}

uint32_t Model::processTexture(
    const aiMaterial* material,
    const aiTextureType& type,
    const aiScene* scene) {
    uint32_t texture = 0;

    //for texture
    aiString aiPath;
    Image* image = nullptr;

    material->Get(AI_MATKEY_TEXTURE(type, 0), aiPath);

    if (!aiPath.length) {
        return 0;
    }

    //先检查缓存是否有纹理
    auto iter = mTextureCache.find(std::string(aiPath.C_Str()));
    if (iter != mTextureCache.end()) {
        return iter->second;
    }

    const aiTexture* assimpTexture = scene->GetEmbeddedTexture(aiPath.C_Str());

    if (assimpTexture) {
        //如果确实图片打包在了模型内部，则上述代码获取到的aiTexture里面就含有了图片数据
        unsigned char* dataIn = reinterpret_cast<unsigned char*>(assimpTexture->pcData);
        uint32_t widthIn = assimpTexture->mWidth;
        uint32_t heightIn = assimpTexture->mHeight;
        std::string path = aiPath.C_Str();
        Image* image = Image::createImageFromMemory(path, dataIn, widthIn, heightIn);

        texture = createTexture(image);

        Image::destroyImage(image);
    }
    else {
        std::string fullPath = mRootPath + aiPath.C_Str();
        Image* image = Image::createImage(fullPath);

        texture = createTexture(image);

        Image::destroyImage(image);
    }

    mTextureCache.insert(std::make_pair(aiPath.C_Str(), texture));

    return texture;
}

uint32_t Model::createTexture(Image* image) {
    uint32_t texture = sgl->genTexture();
    sgl->bindTexture(texture);
    sgl->texImage2D(image->mWidth, image->mHeight, image->mData);
    sgl->texParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
    sgl->texParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
    sgl->texParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
    sgl->bindTexture(0);

    return texture;
}

math::Mat4f Model::getMat4f(aiMatrix4x4 value) {
    math::Mat4f to;

    //the a, b, c, d in assimp is the row
    to.set(
        value.a1, value.a2, value.a3, value.a4,
        value.b1, value.b2, value.b3, value.b4,
        value.c1, value.c2, value.c3, value.c4,
        value.d1, value.d2, value.d3, value.d4
    );

    return to;
}