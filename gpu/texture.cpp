#include "texture.h"

void Texture::setBufferData(
    const uint32_t& width,
    const uint32_t& height,
    void* data) {

    if (mBuffer != nullptr) {
        delete[] mBuffer;
        mBuffer = nullptr;
    }

    mWidth = width;
    mHeight = height;

    if (mBuffer == nullptr) {
        mBuffer = new RGBA[mWidth * mHeight];
    }

    memcpy(mBuffer, data, mWidth * mHeight * sizeof(RGBA));
}

math::vec4f Texture::getColor(float u, float v) {
    RGBA resColor;

    //´¦Àíuv×ø±ê
    checkWrap(u, mWrapU);
    checkWrap(v, mWrapV);

    if (mFilter == TEXTURE_FILTER_NEAREST) {
        int x = std::round(u * (mWidth - 1));
        int y = std::round(v * (mHeight - 1));

        int position = y * mWidth + x;
        resColor = mBuffer[position];
    }
    else if (mFilter == TEXTURE_FILTER_LINEAR) {
        float x = u * static_cast<float>(mWidth - 1);
        float y = v * static_cast<float>(mHeight - 1);

        int left = std::floor(x);
        int right = std::ceil(x);
        int bottom = std::floor(y);
        int top = std::ceil(y);

        float yScale = top == bottom ? 1.0 : (top - y) / (top - bottom);
        float xScale = left == right ? 1.0 : (x - left) / (right - left);

        int posLeftTop = top * mWidth + left;
        int posLeftBottom = bottom * mWidth + left;
        int posRightTop = top * mWidth + right;
        int posRightBottom = bottom * mWidth + right;

        RGBA l = math::lerp(mBuffer[posLeftTop], mBuffer[posLeftBottom], yScale);
        RGBA r = math::lerp(mBuffer[posRightTop], mBuffer[posRightBottom], yScale);
        resColor = math::lerp(l, r, xScale);

        math::vec4f result;
        result.x = static_cast<float>(resColor.mR) / 255.0;
        result.y = static_cast<float>(resColor.mG) / 255.0;
        result.z = static_cast<float>(resColor.mB) / 255.0;
        result.w = static_cast<float>(resColor.mA) / 255.0;

        return result;
    }
}

void Texture::checkWrap(float& n, const uint32_t& type) {
    if (n > 1.0f || n < 0.0f) {
        n = FRACTION(n);
        switch (type) {
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

void Texture::setParameter(const uint32_t& type, const uint32_t& value) {
    switch (type) {
    case TEXTURE_FILTER:
        mFilter = value;
        break;
    case TEXTURE_WRAP_U:
        mWrapU = value;
        break;
    case TEXTURE_WRAP_V:
        mWrapV = value;
        break;
    default:
        break;
    }
}