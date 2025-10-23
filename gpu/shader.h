#pragma once
#include "../global/base.h"
#include "dataStructure.h"
#include "bufferObject.h"
#include "./texture.h"
#include<algorithm>

class Shader {
public:
    Shader() {}
    ~Shader() {}

    virtual VsOutput vertexShader(
        //VAO���е�bindingMap
        const std::map<uint32_t, BindingDescription>& bindingMap,

        //VBO���е�bindingMap
        const std::map<uint32_t, BufferObject*>& bufferMap,

        //��ǰҪ����Ķ����index
        const uint32_t& index
    ) = 0;

    virtual void fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) = 0;
public:
    math::vec4f getVector(
        const std::map<uint32_t, BindingDescription>& bindingMap,
        const std::map<uint32_t, BufferObject*>& bufferMap,
        const uint32_t& attributeLocation, //��ǰ���Եı��
        const uint32_t& index); //��ǰ������

    RGBA vectorToRGBA(const math::vec4f& v) {
        RGBA color;
        //��ֹ��ɫԽ��
        math::vec4f c = v;
        c.x = std::clamp(c.x, 0.0f, 1.0f);
        c.y = std::clamp(c.y, 0.0f, 1.0f);
        c.z = std::clamp(c.z, 0.0f, 1.0f);
        c.w = std::clamp(c.w, 0.0f, 1.0f);
        color.mR = c.x * 255.0;
        color.mG = c.y * 255.0;
        color.mB = c.z * 255.0;
        color.mA = c.w * 255.0;

        return color;
    }
};