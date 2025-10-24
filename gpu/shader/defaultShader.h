#pragma once
#include "../shader.h"
#include "../../math/math.h"

class DefaultShader : public Shader {
public:
	DefaultShader();
	~DefaultShader();

	VsOutput vertexShader(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& index
	) override;

	void fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) override;

public:
	// 公用数据段
	math::Mat4f mModelMatrix;
	math::Mat4f mViewMatrix;
	math::Mat4f mProjectionMatrix;
};