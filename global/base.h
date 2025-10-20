#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <assert.h>

#define PI               3.14159265358979323
#define DEG2RAD(theta)   (0.01745329251994329 * (theta))
#define FRACTION(v)      ((v) - (int)(v))

// #include "../math/math.h"

using byte = unsigned char;

struct RGBA {
    byte mB;
    byte mG;
    byte mR;
    byte mA;

    RGBA(
        byte r = 255,
        byte g = 255,
        byte b = 255,
        byte a = 255
    )
    {
        mR = r;
        mG = g;
        mB = b;
        mA = a;
    }
};

#include "../math/math.h"

struct Point {
    int32_t x;
    int32_t y;
    RGBA color;
    math::vec2f uv;
};

#define TEXTURE_WRAP_REPEAT 0
#define TEXTURE_WRAP_MIRROR 1

#define ARRAY_BUFFER 0
#define ELEMENT_ARRAY_BUFFER 1

#define DRAW_LINES 0
#define DRAW_TRIANGLES 1