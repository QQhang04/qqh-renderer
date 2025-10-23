#include "gpu.h"
#include "raster.h"

GPU* GPU::mInstance = nullptr;

GPU* GPU::getInstance() {
    if (mInstance == nullptr) {
        mInstance = new GPU();
    }
    return mInstance;
}

GPU::GPU() {}
GPU::~GPU() {
    if (mFrameBuffer) {
        delete mFrameBuffer;
    }
}

void GPU::initSurface(const uint32_t& width, const uint32_t& height, void* buffer) {
    mFrameBuffer = new FrameBuffer(width, height, buffer);
    mScreenMatrix = math::screenMatrix<float>(width - 1, height - 1);
}

void GPU::clear() {
    size_t pixelSize = mFrameBuffer->mWidth * mFrameBuffer->mHeight;
    std::fill_n(mFrameBuffer->mColorBuffer, pixelSize, RGBA(0, 0, 0, 0));
    std::fill_n(mFrameBuffer->mDepthBuffer, pixelSize, 1.0f);
}

// 数据层接口
uint32_t GPU::genBuffer() {
    mBufferCounter++;
    mBufferMap.insert(std::make_pair(mBufferCounter, new BufferObject()));

    return mBufferCounter;
}

void GPU::deleteBuffer(const uint32_t& bufferID) {
    auto iter = mBufferMap.find(bufferID);
    if (iter != mBufferMap.end()) {
        delete iter->second;
    }

    mBufferMap.erase(iter);
}

void GPU::bindBuffer(const uint32_t& bufferType, const uint32_t& bufferID) {
    if (bufferType == ARRAY_BUFFER) {
        mCurrentVBO = bufferID;
    }
    else if (bufferType == ELEMENT_ARRAY_BUFFER) {
        mCurrentEBO = bufferID;
    }
}

void GPU::bufferData(const uint32_t& bufferType, size_t dataSize, void* data) {
    uint32_t bufferID = 0;
    if (bufferType == ARRAY_BUFFER) {
        bufferID = mCurrentVBO;
    }
    else if (bufferType == ELEMENT_ARRAY_BUFFER) {
        bufferID = mCurrentEBO;
    }
    else {
        assert(false);
    }
    auto iter = mBufferMap.find(bufferID);
    if (iter == mBufferMap.end()) {
        assert(false);
    }

    BufferObject* bufferObject = iter->second;
    bufferObject->setBufferData(dataSize, data);
}

uint32_t GPU::genVertexArray() {
    mVaoCounter++;
    mVaoMap.insert(std::make_pair(mVaoCounter, new VertexArrayObject()));

    return mVaoCounter;
}

void GPU::deleteVertexArray(const uint32_t& vaoID) {
    auto iter = mVaoMap.find(vaoID);
    if (iter != mVaoMap.end()) {
        delete iter->second;
    }
    else {
        return;
    }
}

void GPU::bindVertexArray(const uint32_t& vaoID) {
    mCurrentVAO = vaoID;
}

void GPU::vertexAttributePointer(
    const uint32_t& binding,
    const uint32_t& itemSize,
    const uint32_t& stride,
    const uint32_t& offset)
{
    auto iter = mVaoMap.find(mCurrentVAO);
    if (iter == mVaoMap.end()) {
        assert(false);
    }

    auto vao = iter->second;
    vao->set(binding, mCurrentVBO, itemSize, stride, offset);
}

void GPU::printVAO(const uint32_t& vaoID) {
    auto iter = mVaoMap.find(vaoID);
    if (iter != mVaoMap.end()) {
        iter->second->print();
    }
}

void GPU::printEBO(const uint32_t& eboID) {
    auto iter = mBufferMap.find(eboID);
    if (iter != mBufferMap.end()) {
        byte* data = iter->second->getBuffer();
        for (int i = 0; i < 3; i++) {
            int tmp;
            memcpy(&tmp, data + i * sizeof(uint32_t), sizeof(uint32_t));
            std::cout << "EBO:" << tmp << " ";
        }
    }
    std::cout << std::endl;
}

// 绘制接口
void GPU::drawElement(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count) {
    if (mCurrentVAO == 0 || mShader == nullptr || count == 0) {
        return;
    }

    // 1 get vao
    auto vaoIter = mVaoMap.find(mCurrentVAO);
    if (vaoIter == mVaoMap.end()) {
        std::cerr << "Error: current vao is invalid!" << std::endl;
        return;
    }

    const VertexArrayObject* vao = vaoIter->second;
    auto bindingMap = vao->getBindingMap();

    // 2 get ebo
    auto eboIter = mBufferMap.find(mCurrentEBO);
    if (eboIter == mBufferMap.end()) {
        std::cerr << "Error: current ebo is invalid!" << std::endl;
        return;
    }
    const BufferObject* ebo = eboIter->second;

    // 3 vertex shader处理阶段
    std::vector<VsOutput> vsOutputs;
    vertexShaderStage(vsOutputs, vao, ebo, first, count);

    if (vsOutputs.empty()) return;

    // 4 Clip裁剪
    std::vector<VsOutput> clipOutputs;
    Clipper::doClipSpace(DRAW_TRIANGLES, vsOutputs, clipOutputs);

    // 4 NDC处理阶段
    for (auto& output : clipOutputs) {
        perspectiveDivision(output);
    }

    // 背面剔除阶段
    std::vector<VsOutput> cullOutputs = clipOutputs;
    if (drawMode == DRAW_TRIANGLES && mEnableCullFace) {
        cullOutputs.clear();
        for (uint32_t i = 0; i < clipOutputs.size() - 2; i += 3) {
            if (Clipper::cullFace(mFrontFace, mCullFace, clipOutputs[i], clipOutputs[i + 1], clipOutputs[i + 2])) {
                auto start = clipOutputs.begin() + i;
                auto end = clipOutputs.begin() + i + 3;
                cullOutputs.insert(cullOutputs.end(), start, end);
            }
        }
    }

    // 5 屏幕映射
    for (auto& output : cullOutputs) {
        screenMapping(output);
    }

    // 6 光栅化离散出需要的fragment
    std::vector<VsOutput> rasterOutputs;
    Raster::rasterize(rasterOutputs, drawMode, cullOutputs);

    if (rasterOutputs.empty()) return;

    // 6.5 透视恢复阶段
    for (auto& out : rasterOutputs) {
        perspectiveRecover(out);
    }
    
    // 7 片元着色器 颜色输出
    FsOutput fsOutput;
    uint32_t pixelPos = 0;
    for (uint32_t i = 0; i < rasterOutputs.size(); ++i) {
        mShader->fragmentShader(rasterOutputs[i], fsOutput, mTextureMap);

        // 进行深度测试
        if (mEnableDepthTest && !depthTest(fsOutput)) {
            continue;
        }

        // 进行颜色混合
        RGBA color = fsOutput.mColor;
        if (mEnableBlending) {
            color = blend(fsOutput);
        }

        pixelPos = fsOutput.mPixelPos.y * mFrameBuffer->mWidth + fsOutput.mPixelPos.x;
        mFrameBuffer->mColorBuffer[pixelPos] = color;
    }
}

void GPU::vertexShaderStage(
    std::vector<VsOutput>& vsOutputs,
    const VertexArrayObject* vao,
    const BufferObject* ebo,
    const uint32_t first,
    const uint32_t count) {

    auto bindingMap = vao->getBindingMap();
    byte* indicesData = ebo->getBuffer();

    uint32_t index = 0;
    for (uint32_t i = first; i < first + count; ++i) {
        //获取Ebo中第i个index
        size_t indicesOffset = i * sizeof(uint32_t);
        memcpy(&index, indicesData + indicesOffset, sizeof(uint32_t));

        VsOutput output = mShader->vertexShader(bindingMap, mBufferMap, index);
        vsOutputs.push_back(output);
    }
}

void GPU::perspectiveDivision(VsOutput& vsOutput) {
    vsOutput.mOneOverW = 1.0f / vsOutput.mPosition.w;

    vsOutput.mPosition *= vsOutput.mOneOverW;
    vsOutput.mColor *= vsOutput.mOneOverW;
    vsOutput.mUV *= vsOutput.mOneOverW;
    vsOutput.mPosition.w = 1.0f;

    trim(vsOutput);
}

void GPU::perspectiveRecover(VsOutput& vsOutput) {
    vsOutput.mColor /= vsOutput.mOneOverW;
    vsOutput.mUV /= vsOutput.mOneOverW;
}

void GPU::trim(VsOutput& vsOutput) {
    // 修剪毛刺，边界求交点的时候，可能会产生超过-1~1现象
    if (vsOutput.mPosition.x < -1.0f) {
        vsOutput.mPosition.x = -1.0f;
    }

    if (vsOutput.mPosition.x > 1.0f) {
        vsOutput.mPosition.x = 1.0f;
    }

    if (vsOutput.mPosition.y < -1.0f) {
        vsOutput.mPosition.y = -1.0f;
    }

    if (vsOutput.mPosition.y > 1.0f) {
        vsOutput.mPosition.y = 1.0f;
    }

    if (vsOutput.mPosition.z < -1.0f) {
        vsOutput.mPosition.z = -1.0f;
    }

    if (vsOutput.mPosition.z > 1.0f) {
        vsOutput.mPosition.z = 1.0f;
    }
}

void GPU::screenMapping(VsOutput& vsOutput) {
    vsOutput.mPosition = mScreenMatrix * vsOutput.mPosition;
}

bool GPU::depthTest(const FsOutput& output) {
    uint32_t pixelPos = output.mPixelPos.y * mFrameBuffer->mWidth + output.mPixelPos.x;
    float oldDepth = mFrameBuffer->mDepthBuffer[pixelPos];
    switch (mDepthFunc)
    {
    case DEPTH_LESS:
        if (output.mDepth < oldDepth) {
            mFrameBuffer->mDepthBuffer[pixelPos] = output.mDepth;
            return true;
        }
        else {
            return false;
        }
        break;
    case DEPTH_GREATER:
        if (output.mDepth > oldDepth) {
            mFrameBuffer->mDepthBuffer[pixelPos] = output.mDepth;
            return true;
        }
        else {
            return false;
        }
        break;
    
    default:
        return false;
        break;
    }
}

RGBA GPU::blend(const FsOutput& output) {
    RGBA result;

    uint32_t pixelPos = output.mPixelPos.y * mFrameBuffer->mWidth + output.mPixelPos.x;
    RGBA dst = mFrameBuffer->mColorBuffer[pixelPos];
    RGBA src = output.mColor;

    float weight = static_cast<float>(src.mA) / 255.0f;

    result.mR = static_cast<float>(src.mR) * weight + static_cast<float>(dst.mR) * (1.0f - weight);
    result.mG = static_cast<float>(src.mG) * weight + static_cast<float>(dst.mG) * (1.0f - weight);
    result.mB = static_cast<float>(src.mB) * weight + static_cast<float>(dst.mB) * (1.0f - weight);
    result.mA = static_cast<float>(src.mA) * weight + static_cast<float>(dst.mA) * (1.0f - weight);

    return result;
}

// texture:
void GPU::texImage2D(const uint32_t& width, const uint32_t& height, void* data) {
    if (!mCurrentTexture) {
        return;
    }

    auto iter = mTextureMap.find(mCurrentTexture);
    if (iter == mTextureMap.end()) {
        return;
    }
    auto texture = iter->second;
    texture->setBufferData(width, height, data);
}

void GPU::deleteTexture(const uint32_t& texID) {
    auto iter = mTextureMap.find(texID);
    if (iter != mTextureMap.end()) {
        delete iter->second;
    }
    else {
        return;
    }

    mTextureMap.erase(iter);
}

uint32_t GPU::genTexture() {
    mTextureCounter++;
    mTextureMap.insert(std::make_pair(mTextureCounter, new Texture()));

    return mTextureCounter;
}

void GPU::texParameter(const uint32_t& param, const uint32_t& value) {
    if (!mCurrentTexture) {
        return;
    }

    auto iter = mTextureMap.find(mCurrentTexture);
    if (iter == mTextureMap.end()) {
        return;
    }
    auto texture = iter->second;
    texture->setParameter(param, value);
}

void GPU::bindTexture(const uint32_t& texID) {
    mCurrentTexture = texID;
}



