#include "raster.h"

void Raster::rasterizeLine(
    std::vector<Point>& results,
    const Point& v0,
    const Point& v1
) {
    results.clear();
    Point start = v0;
    Point end = v1;

    // 验证x也是从小到大，需要交换x和y，并记录交换状态
    if (start.x > end.x) {
        Point tmp = start;
        start = end;
        end = tmp;
    }

    results.push_back(start);

    //2 验证y也是从小到大，需要翻转y，并记录翻转状态
    bool flipY = false;
    if (start.y > end.y) {
        start.y *= -1.0f;
        end.y *= -1.0f;
        flipY = true;
    }

    //3 验证斜率在0-1之间，需要交换x和y，并记录交换状态
    int deltaX = static_cast<int>(end.x - start.x);
    int deltaY = static_cast<int>(end.y - start.y);

    bool swapXY = false;
    if (deltaX < deltaY) {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        std::swap(deltaX, deltaY);
        swapXY = true;
    }

    int curX = static_cast<int>(start.x);
    int curY = static_cast<int>(start.y);

    int resX = 0, resY = 0;
    Point currentPoint;
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

        currentPoint.x = resX;
        currentPoint.y = resY;
        results.push_back(currentPoint);
    }
}