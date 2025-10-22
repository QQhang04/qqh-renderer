#pragma once
#include "../application/application.h"
#include "./frameBuffer.h"
#include "../application/image.h"
#include "./vao.h"
#include "./bufferObject.h"
#include "./shader/defaultShader.h"
#include "./clipper.h"

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
    void screenMapping(VsOutput& vsOutput);
    void perspectiveRecover(VsOutput& vsOutput);
    /* 绘制相关接口 deprecated
    void drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color);
    void drawLine(const Point& p1, const Point& p2);
    void drawTriangle(const Point& p1, const Point& p2, const Point& p3);
    void drawImage(const Image* image);
    void drawImageWithAlpha(const Image* image, const uint32_t& alpha);

    void checkWrap(float& n);
    */

private:
    // uv坐标采样
    /*RGBA sampleNearest(const math::vec2f& uv);
    RGBA sampleBilinear(const math::vec2f& uv);

    int getImagePosition(Image* image, int x, int y) {return y * image->mWidth + x;}*/

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
};