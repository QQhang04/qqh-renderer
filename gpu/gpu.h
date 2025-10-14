#pragma once
#include "../application/application.h"
#include "./frameBuffer.h"
#include "../application/image.h"

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

    // 绘制相关接口
    void drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color);
    void drawLine(const Point& p1, const Point& p2);
    void drawTriangle(const Point& p1, const Point& p2, const Point& p3);
    void drawImage(const Image* image);
    void drawImageWithAlpha(const Image* image, const uint32_t& alpha);

    void setBlending(bool isEnabled) { mEnableBlending = isEnabled; }
    void setBilinear(bool isEnabled) { mEnableBilinear = isEnabled; }
    void setTexture(Image* image) { mImage = image; }
    void setTextureWrap(uint32_t wrap) { mWrap = wrap; }

    void checkWrap(float& n);

private:
    RGBA sampleNearest(const math::vec2f& uv);
    RGBA sampleBilinear(const math::vec2f& uv);

    int getImagePosition(Image* image, int x, int y) {return y * image->mWidth + x;}

private:
    static GPU* mInstance;

    bool mEnableBlending = false;
    bool mEnableBilinear = false;
    uint32_t mWrap{ TEXTURE_WRAP_REPEAT };

    FrameBuffer* mFrameBuffer{ nullptr };
    Image* mImage{ nullptr }; // 纹理贴图
};