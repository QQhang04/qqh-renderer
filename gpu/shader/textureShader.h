#include "shader.h"
class TextureShader : public Shader {
public:
    TextureShader();
    ~TextureShader();

    VsOutput vertexShader(
        const std::map<uint32_t, BindingDescription>& bindingMap,
        const std::map<uint32_t, BufferObject*>& bufferMap,
        const uint32_t& index
    ) override;

    void fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& tex) override;

public:
    //uniforms
    math::Mat4f mModelMatrix;
    math::Mat4f mViewMatrix;
    math::Mat4f mProjectionMatrix;

    uint32_t mDiffuseTexture{ 0 };
};