#pragma once
#include "../application/image.h"
#include "../math/math.h"
#include "../global/base.h"

class Texture {
public:
    Texture() {};
    ~Texture() {
        if (mBuffer) {
            delete[] mBuffer;
            mBuffer = nullptr;
        }
    };

    //Ĭ�϶���rgba��ʽ
    void setBufferData(const uint32_t& width, const uint32_t& height, void* data);

    math::vec4f getColor(float u, float v);

    void setParameter(const uint32_t& type, const uint32_t& value);

private:
    void checkWrap(float& n, const uint32_t& type);

private:
    RGBA* mBuffer{ nullptr };
    uint32_t mWidth{ 0 };
    uint32_t mHeight{ 0 };

    uint32_t mFilter{ TEXTURE_FILTER_NEAREST };
    uint32_t mWrapU{ TEXTURE_WRAP_REPEAT };
    uint32_t mWrapV{ TEXTURE_WRAP_REPEAT };
};