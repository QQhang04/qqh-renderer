#include "shader.h"

math::vec4f Shader::getVector(
    const std::map<uint32_t, BindingDescription>& bindingMap,
    const std::map<uint32_t, BufferObject*>& bufferMap,
    const uint32_t& attributeLocation, //当前属性的编号
    const uint32_t& index) {
    auto desIter = bindingMap.find(attributeLocation);
    if (desIter == bindingMap.end()) {
        assert(false);
    }

    auto description = desIter->second;
    auto vboId = description.mVbold;
    auto vboIter = bufferMap.find(vboId);
    if (vboIter == bufferMap.end()) {
        assert(false);
    }

    BufferObject* object = vboIter->second;

    uint32_t dataStart = description.mStride * index + description.mOffset;
    uint32_t dataSize = description.mItemSize * sizeof(float);

    math::vec4f res;
    const byte* buffer = object->getBuffer() + dataStart;
    memcpy(&res, buffer, dataSize);

    return res;
}