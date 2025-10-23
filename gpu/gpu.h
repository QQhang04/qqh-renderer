#pragma once
#include "../application/application.h"
#include "./frameBuffer.h"
#include "../application/image.h"
#include "./vao.h"
#include "./bufferObject.h"
#include "./shader/defaultShader.h"
#include "./shader/textureShader.h"
#include "./clipper.h"
#include "./texture.h"

#define sgl GPU::getInstance()

/*
 * class GPU:
 * 模拟GPU的绘图行为以及算法
 */
class GPU {
public:
    static GPU* getInstance();
    GPU();
    ~GPU();

    // 接受外界传入的bmp对应的内存指针以及窗体的宽/高
    void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

    // 清除画布内容
    void clear();

    // 数据层相关接口
    uint32_t genBuffer();
    void deleteBuffer(const uint32_t& bufferID);
    void bindBuffer(const uint32_t& bufferType, const uint32_t& bufferID);
    void bufferData(const uint32_t& bufferType, size_t dataSize, void* data);

    uint32_t genVertexArray();
    void deleteVertexArray(const uint32_t& vaoID);
    void bindVertexArray(const uint32_t& vaoID);
    void vertexAttributePointer(
        const uint32_t& binding,
        const uint32_t& itemSize,
        const uint32_t& stride,
        const uint32_t& offset);

    void printVAO(const uint32_t& vaoID);
    void printEBO(const uint32_t& eboID);

    // shader绘制相关接口
    void useProgram(Shader* shader) {
        mShader = shader;
    }

    void drawElement(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count);
    void vertexShaderStage(std::vector<VsOutput>& vsOutputs,
        const VertexArrayObject* vao,
        const BufferObject* ebo,
        const uint32_t first,
        const uint32_t count);
    void perspectiveDivision(VsOutput& vsOutput);
    void trim(VsOutput& vsOutput);
    bool depthTest(const FsOutput& output);
    void screenMapping(VsOutput& vsOutput);
    void perspectiveRecover(VsOutput& vsOutput);
    RGBA blend(const FsOutput& output);

    // texture设置相关接口
    void texImage2D(const uint32_t& width, const uint32_t& height, void* data);
    void deleteTexture(const uint32_t& texID);
    uint32_t genTexture();
    void texParameter(const uint32_t& param, const uint32_t& value);
    void bindTexture(const uint32_t& texID);

    // 设置参数相关接口
    void setEnable(const uint32_t& value, bool val) {
        switch (value) {
        case CULL_FACE:
            mEnableCullFace = val;
            break;
        case DEPTH_TEST:
            mEnableDepthTest = val;
            break;
        default:
            break;
        }
    }

    void GPU::frontFace(const uint32_t& value) {
        mFrontFace = value;
    }

    void GPU::cullFace(const uint32_t& value) {
        mCullFace = value;
    }

    void GPU::depthFunc(const uint32_t& value) {
        mDepthFunc = value;
    }

private:
    static GPU* mInstance;
    FrameBuffer* mFrameBuffer{ nullptr };

    //VBO相关
    uint32_t mBufferCounter{ 0 };
    uint32_t mCurrentVBO{ 0 };
    uint32_t mCurrentEBO{ 0 };
    std::map<uint32_t, BufferObject*> mBufferMap;

    //VAO相关
    uint32_t mVaoCounter{ 0 };
    uint32_t mCurrentVAO{ 0 };
    std::map<uint32_t, VertexArrayObject*> mVaoMap;

    // Shader和绘制相关
    Shader* mShader { nullptr };
    math::Mat4f mScreenMatrix;

    // cull相关
    bool mEnableCullFace{ true };
    uint32_t mFrontFace{ BACK_FACE }; // 逆时针为正面
    uint32_t mCullFace{ FRONT_FACE_CCW }; // 背面剔除

    // 深度测试相关
    bool mEnableDepthTest{ true };
    uint32_t mDepthFunc{ DEPTH_LESS }; 

    // 颜色混合相关
    bool mEnableBlending{ true };

    // texture纹理管理相关
    uint32_t mCurrentTexture{ 0 };
    uint32_t mTextureCounter{ 0 };
    std::map<uint32_t, Texture*> mTextureMap;
};