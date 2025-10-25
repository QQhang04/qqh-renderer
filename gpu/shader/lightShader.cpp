#include "lightShader.h"

LightShader::LightShader() {}
LightShader::~LightShader() {}

VsOutput LightShader::vertexShader(
	const std::map<uint32_t, BindingDescription>& bindingMap,
	const std::map<uint32_t, BufferObject*>& bufferMap,
	const uint32_t& index
) {
	VsOutput output;

	math::vec4f position = getVector(bindingMap, bufferMap, 0, index);
	position.w = 1.0f; //变化为齐次坐标
	//math::vec4f color = getVector(bindingMap, bufferMap, 1, index);
	math::vec3f normal = getVector(bindingMap, bufferMap, 1, index);
	math::vec2f uv = getVector(bindingMap, bufferMap, 2, index);

	output.mPosition = mProjectionMatrix * mViewMatrix * mModelMatrix * position;
	output.mUV = uv;
	output.mNormal = normal;

	return output;
}

void LightShader::fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) {
	output.mPixelPos.x = static_cast<int>(input.mPosition.x);
	output.mPixelPos.y = static_cast<int>(input.mPosition.y);
	output.mDepth = input.mPosition.z;

	//取出法线
	auto normal = math::normalize(input.mNormal);
	auto lightDirection = math::normalize(mDirectionalLight.direction);

	//取出texture
	Texture* currentTex;
	if (textures.size() != 0)
	{
		auto iter = textures.find(mDiffuseTexture);
		currentTex = iter->second;
	}
	else
	{
		currentTex = nullptr;
	}

	//计算颜色
	math::vec4f texColor = currentTex != nullptr ? currentTex->getColor(input.mUV.x, input.mUV.y) : math::vec4f(1, 1, 1, 1);

	//计算漫反射光
	math::vec4f diffuseColor;
	float diff = math::dot(normal, -lightDirection);
	diff = std::clamp(diff, 0.0f, 1.0f);
	diffuseColor = texColor * diff * math::vec4f(mDirectionalLight.color, 1.0f);

	//计算环境光
	math::vec4f envColor;
	envColor = texColor * math::vec4f(mEnvLight, 1.0f);
	output.mColor = vectorToRGBA(diffuseColor + envColor);
}