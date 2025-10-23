#include "textureShader.h"

TextureShader::TextureShader() {}
TextureShader::~TextureShader() {}

VsOutput TextureShader::vertexShader(
    const std::map<uint32_t, BindingDescription>& bindingMap,
    const std::map<uint32_t, BufferObject*>& bufferMap,
    const uint32_t& index
) {
    VsOutput output;

    math::vec4f position = getVector(bindingMap, bufferMap, 0, index);
    position.w = 1.0f; //变化为齐次坐标
    math::vec4f color = getVector(bindingMap, bufferMap, 1, index);
    math::vec2f uv = getVector(bindingMap, bufferMap, 2, index);

    output.mPosition = mProjectionMatrix * mViewMatrix * mModelMatrix * position;
    output.mColor = color;
    output.mUV = uv;

    return output;
}

void TextureShader::fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) {
    output.mPixelPos.x = static_cast<int>(input.mPosition.x);
    output.mPixelPos.y = static_cast<int>(input.mPosition.y);
    output.mDepth = input.mPosition.z;

    auto iter = textures.find(mDiffuseTexture);
    auto texture = iter->second;

    math::vec4f diffuseColor = texture->getColor(input.mUV.x, input.mUV.y);
    // 纹理颜色与顶点颜色相乘得到最终颜色，也可以直接使用漫反射颜色
    math::vec4f finalColor = diffuseColor;
    output.mColor = vectorToRGBA(finalColor);
}