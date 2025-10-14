#include "gpu.h"
#include "raster.h"

GPU* GPU::mInstance = nullptr;

GPU* GPU::getInstance() {
    if (mInstance == nullptr) {
        mInstance = new GPU();
    }
    return mInstance;
}

GPU::GPU() {}
GPU::~GPU() {
    if (mFrameBuffer) {
        delete mFrameBuffer;
    }
}

void GPU::initSurface(const uint32_t& width, const uint32_t& height, void* buffer) {
    mFrameBuffer = new FrameBuffer(width, height, buffer);
}

void GPU::clear() {
    size_t pixelSize = mFrameBuffer->mWidth * mFrameBuffer->mHeight;
    std::fill_n(mFrameBuffer->mColorBuffer, pixelSize, RGBA(0, 0, 0, 0));
}

void GPU::drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color) {
    // 从窗口左下角开始
    if (x >= app->getWidth() || y >= app->getHeight()) {
        return;
    }
    uint32_t pixelPos = y * mFrameBuffer->mWidth + x;

    RGBA result = color;
    if (mEnableBlending) {
        // 加入blending
        auto src = color;
        auto dst = mFrameBuffer->mColorBuffer[pixelPos];
        float weight = static_cast<float>(src.mA) / 255.0f;
        result = Raster::lerpRGBA(dst, src, weight);
    }
    mFrameBuffer->mColorBuffer[pixelPos] = result;
}

void GPU::drawLine(const Point& p1, const Point& p2) {
    std::vector<Point> pixels;
    Raster::rasterizeLine(pixels, p1, p2);

    for (auto& p : pixels) {
        drawPoint(p.x, p.y, p.color);
    }
}

void GPU::drawTriangle(const Point& p1, const Point& p2, const Point& p3) {
    std::vector<Point> pixels;
    Raster::rasterizeTriangle(pixels, p1, p2, p3);

    RGBA resColor;
    for (auto& p : pixels) {
        if (mImage) {
            resColor = mEnableBilinear ? sampleBilinear(p.uv) : sampleNearest(p.uv);
        }
        else {
            resColor = p.color;
        }
        drawPoint(p.x, p.y, resColor);
    }
}

void GPU::drawImage(const Image* image) {
    for (uint32_t i = 0; i < image->mWidth; ++i) {
        for (uint32_t j = 0; j < image->mHeight; ++j) {
            drawPoint(i, j, image->mData[j * image->mWidth + i]);
        }
    }
}

void GPU::drawImageWithAlpha(const Image* image, const uint32_t& alpha) {
    RGBA color;
    for (uint32_t i = 0; i < image->mWidth; ++i) {
        for (uint32_t j = 0; j < image->mHeight; ++j) {
            color = image->mData[j * image->mWidth + i];
            color.mA = alpha;
            drawPoint(i, j, color);
        }
    }
}

RGBA GPU::sampleNearest(const math::vec2f& uv) {
    auto myUV = uv;
    checkWrap(myUV.x); checkWrap(myUV.y);
    // 四舍五入到最近整数
    // u = 0 对应 x = 0，u = 1 对应 x = width - 1
    // v = 0 对应 y = 0，v = 1 对应 y = height - 1
    int x = std::round(myUV.x * (mImage->mWidth - 1));
    int y = std::round(myUV.y * (mImage->mHeight - 1));

    int position = y * mImage->mWidth + x;
    return mImage->mData[position];
}

RGBA GPU::sampleBilinear(const math::vec2f& uv) {
    RGBA resColor;
    auto myuv = uv; // 相当于拷贝了一份 
    checkWrap(myuv.x); checkWrap(myuv.y);

    float x = uv.x * static_cast<float>(mImage->mWidth - 1);
    float y = uv.y * static_cast<float>(mImage->mHeight - 1);

    int left = std::floor(x);
    int right = std::ceil(x);
    int bottom = std::floor(y);
    int top = std::ceil(y);

    float yScale = top == bottom ? 1.0 : (top - y) / (top - bottom);
    float xScale = left == right ? 1.0 : (x - left) / (right - left);

    int posLeftTop = getImagePosition(mImage, left, top);
    int posLeftBottom = getImagePosition(mImage, left, bottom);
    int posRightTop = getImagePosition(mImage, right, top);
    int posRightBottom = getImagePosition(mImage, right, bottom);

    RGBA l = Raster::lerpRGBA(mImage->mData[posLeftTop], mImage->mData[posLeftBottom], yScale);
    RGBA r = Raster::lerpRGBA(mImage->mData[posRightTop], mImage->mData[posRightBottom], yScale);
    resColor = Raster::lerpRGBA(l, r, xScale);
    return resColor;
}

void GPU::checkWrap(float& n) {
    if (n > 1.0f || n < 0.0f) {
        n = FRACTION(n);
        switch (mWrap) {
        case TEXTURE_WRAP_REPEAT:
            n = FRACTION(n + 1);
            break;
        case TEXTURE_WRAP_MIRROR:
            n = 1.0f - FRACTION(n + 1);
            break;
        default:
            break;
        }
    }
}



