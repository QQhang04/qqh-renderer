#pragma once
#include "../global/base.h"

/*
 * class Raster
 * �����ṩ��̬�������ݽӿڣ�������ɢ��ͼԪ�㣬���ع�դ�������������
 */
class Raster {
public:
    Raster() {};
    ~Raster() {};

    static void rasterizeLine(
        std::vector<Point>& results,
        const Point& v0,
        const Point& v1
    );
};