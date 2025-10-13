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

private:
    static GPU* mInstance;
    bool mEnableBlending = false;
    FrameBuffer* mFrameBuffer{ nullptr };
};