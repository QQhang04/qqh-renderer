#include "raster.h"

void Raster::rasterize(
    std::vector<VsOutput>& results,
    const uint32_t& drawMode,
    const std::vector<VsOutput>& inputs) {

    if (drawMode == DRAW_LINES) {
        for (uint32_t i = 0; i < inputs.size(); i += 2) {
            rasterizeLine(results, inputs[i], inputs[i + 1]);
        }
    }

    if (drawMode == DRAW_TRIANGLES) {
        for (uint32_t i = 0; i < inputs.size(); i += 3) {
            rasterizeTriangle(results, inputs[i], inputs[i + 1], inputs[i + 2]);
        }
    }
}

void Raster::rasterizeLine(
    std::vector<VsOutput>& results,
    const VsOutput& v0,
    const VsOutput& v1
) {
    results.clear();
    auto start = v0;
    auto end = v1;

    // 验证x也是从小到大，需要交换x和y，并记录交换状态
    if (start.mPosition.x > end.mPosition.x) {
        auto tmp = start;
        start = end;
        end = tmp;
    }

    results.push_back(start);

    //2 验证y也是从小到大，需要翻转y，并记录翻转状态
    bool flipY = false;
    if (start.mPosition.y > end.mPosition.y) {
        start.mPosition.y *= -1.0f;
        end.mPosition.y *= -1.0f;
        flipY = true;
    }

    //3 验证斜率在0-1之间，需要交换x和y，并记录交换状态
    int deltaX = static_cast<int>(end.mPosition.x - start.mPosition.x);
    int deltaY = static_cast<int>(end.mPosition.y - start.mPosition.y);

    bool swapXY = false;
    if (deltaX < deltaY) {
        std::swap(start.mPosition.x, start.mPosition.y);
        std::swap(end.mPosition.x, end.mPosition.y);
        std::swap(deltaX, deltaY);
        swapXY = true;
    }

    int curX = static_cast<int>(start.mPosition.x);
    int curY = static_cast<int>(start.mPosition.y);

    int resX = 0, resY = 0;
    VsOutput currentPoint;
    int p = 2 * deltaY - deltaX;

    for (int i = 0; i < deltaX; i++) {
        if (p >= 0) {
            curY++;
            p -= 2 * deltaX;
        }
        p += 2 * deltaY;
        curX += 1;

        // 需要swap和flip
        resX = curX; resY = curY;
        if (swapXY) {
            std::swap(resX, resY);
        }
        if (flipY) {
            resY *= -1;
        }

        currentPoint.mPosition.x = resX;
        currentPoint.mPosition.y = resY;

        interpolantLine(start, end, currentPoint);

        results.push_back(currentPoint);
    }
}

void Raster::interpolantLine(const VsOutput& v0, const VsOutput& v1, VsOutput& target) {
    float weight = 1.0f;
    if (v1.mPosition.x != v0.mPosition.x) {
        //用x做比例
        weight = (float)(target.mPosition.x - v0.mPosition.x) / (float)(v1.mPosition.x - v0.mPosition.x);
    }
    else if (v1.mPosition.y != v0.mPosition.y) {
        //用y做比例
        weight = (float)(target.mPosition.y - v0.mPosition.y) / (float)(v1.mPosition.y - v0.mPosition.y);
    }

    target.mColor = math::lerp(v0.mColor, v1.mColor, weight);
    target.mUV = math::lerp(v0.mUV, v1.mUV, weight);
}
void Raster::rasterizeTriangle(
    std::vector<VsOutput>& results,
    const VsOutput& v0,
    const VsOutput& v1,
    const VsOutput& v2) {
    int maxX = static_cast<int>(std::max(std::max(v0.mPosition.x, v1.mPosition.x), v2.mPosition.x));
    int minX = static_cast<int>(std::min(std::min(v0.mPosition.x, v1.mPosition.x), v2.mPosition.x));
    int maxY = static_cast<int>(std::max(std::max(v0.mPosition.y, v1.mPosition.y), v2.mPosition.y));
    int minY = static_cast<int>(std::min(std::min(v0.mPosition.y, v1.mPosition.y), v2.mPosition.y));

    math::vec2f pv0, pv1, pv2;
    VsOutput result;
    for (int i = minX; i <= maxX; ++i) {
        for (int j = minY; j <= maxY; ++j) {
            pv0 = math::vec2f(v0.mPosition.x - i, v0.mPosition.y - j);
            pv1 = math::vec2f(v1.mPosition.x - i, v1.mPosition.y - j);
            pv2 = math::vec2f(v2.mPosition.x - i, v2.mPosition.y - j);

            auto cross1 = math::cross(pv0, pv1);
            auto cross2 = math::cross(pv1, pv2);
            auto cross3 = math::cross(pv2, pv0);

            bool negativeAll = cross1 < 0 && cross2 < 0 && cross3 < 0;
            bool positiveAll = cross1 > 0 && cross2 > 0 && cross3 > 0;

            if (negativeAll || positiveAll) {
                result.mPosition.x = i;
                result.mPosition.y = j;
                interpolantTriangle(v0, v1, v2, result);
                results.push_back(result);
            }
        }
    }
}

void Raster::interpolantTriangle(const VsOutput& v0, const VsOutput& v1, const VsOutput& v2, VsOutput& p) {
    auto edge1 = math::vec2f(v1.mPosition.x - v0.mPosition.x, v1.mPosition.y - v0.mPosition.y);
    auto edge2 = math::vec2f(v2.mPosition.x - v0.mPosition.x, v2.mPosition.y - v0.mPosition.y);
    float sumArea = std::abs(math::cross(edge1, edge2));

    auto pv0 = math::vec2f(v0.mPosition.x - p.mPosition.x, v0.mPosition.y - p.mPosition.y);
    auto pv1 = math::vec2f(v1.mPosition.x - p.mPosition.x, v1.mPosition.y - p.mPosition.y);
    auto pv2 = math::vec2f(v2.mPosition.x - p.mPosition.x, v2.mPosition.y - p.mPosition.y);

    // 计算v0的权重
    float v0Area = std::abs(math::cross(pv1, pv2));
    float v1Area = std::abs(math::cross(pv0, pv2));
    float v2Area = std::abs(math::cross(pv0, pv1));

    float weight0 = v0Area / sumArea;
    float weight1 = v1Area / sumArea;
    float weight2 = v2Area / sumArea;

    // 进行透视修正准备w0
    p.mOneOverW = math::lerp(v0.mOneOverW, v1.mOneOverW, v2.mOneOverW, weight0, weight1, weight2);
    // 颜色与 UV 插值
    p.mColor = math::lerp(v0.mColor, v1.mColor, v2.mColor, weight0, weight1, weight2);
    p.mUV = math::lerp(v0.mUV, v1.mUV, v2.mUV, weight0, weight1, weight2);
    // 深度值插值
    p.mPosition.z = math::lerp(v0.mPosition.z, v1.mPosition.z, v2.mPosition.z, weight0, weight1, weight2);
    // 对法线的插值
    p.mNormal = math::lerp(v0.mNormal, v1.mNormal, v2.mNormal, weight0, weight1, weight2);
}
