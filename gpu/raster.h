#pragma once
#include "../global/base.h"
#include "../math/math.h"
#include "./dataStructure.h"

/*
 * class Raster
 * �����ṩ��̬�������ݽӿڣ�������ɢ��ͼԪ�㣬���ع�դ�������������
 */
class Raster {
public:
    Raster() {};
    ~Raster() {};

    static void rasterizeLine(
        std::vector<VsOutput>& results,
        const VsOutput& v0,
        const VsOutput& v1
    );

    static void interpolantLine(const VsOutput& v0, const VsOutput& v1, VsOutput& target);

    static void rasterizeTriangle(
        std::vector<VsOutput>& results,
        const VsOutput& v0,
        const VsOutput& v1,
        const VsOutput& v2
    );

    static void interpolantTriangle(const VsOutput& v0, const VsOutput& v1, const VsOutput& v2, VsOutput& p);

    // Lerp�ӿ�
    static RGBA lerpRGBA(const RGBA& c0, const RGBA& c1, float weight); // ָ��c1��weight�����
    static RGBA lerpRGBA(const RGBA& c0, const RGBA& c1, const RGBA& c2, float weight0, float weight1, float weight2);
    static math::vec2f lerpUV(const math::vec2f& uv1, const math::vec2f& uv2, const math::vec2f& uv3, float weight0, float weight1, float weight2);
};