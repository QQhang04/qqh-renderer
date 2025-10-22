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
        mShader->fragmentShader(rasterOutputs[i], fsOutput);

        // 进行深度测试
        if (mEnableDepthTest && !depthTest(fsOutput)) {
            continue;
        }

        pixelPos = fsOutput.mPixelPos.y * mFrameBuffer->mWidth + fsOutput.mPixelPos.x;
        mFrameBuffer->mColorBuffer[pixelPos] = fsOutput.mColor;
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

/* deprecated currently
void GPU::drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color) {
    // 从窗口左下角开始
    if (x >= app->getWidth() || y >= app->getHeight()) {
        return;
    }
    uint32_t pixelPos = y * mFrameBuffer->mWidth + x;

    RGBA result = color;
    if (mEnableBlending) {
        // 加入blending
        auto src = color;
        auto dst = mFrameBuffer->mColorBuffer[pixelPos];
        float weight = static_cast<float>(src.mA) / 255.0f;
        result = Raster::lerpRGBA(dst, src, weight);
    }
    mFrameBuffer->mColorBuffer[pixelPos] = result;
}

void GPU::drawLine(const Point& p1, const Point& p2) {
    std::vector<Point> pixels;
    Raster::rasterizeLine(pixels, p1, p2);

    for (auto& p : pixels) {
        drawPoint(p.x, p.y, p.color);
    }
}

void GPU::drawTriangle(const Point& p1, const Point& p2, const Point& p3) {
    std::vector<Point> pixels;
    Raster::rasterizeTriangle(pixels, p1, p2, p3);

    RGBA resColor;
    for (auto& p : pixels) {
        if (mImage) {
            resColor = mEnableBilinear ? sampleBilinear(p.uv) : sampleNearest(p.uv);
        }
        else {
            resColor = p.color;
        }
        drawPoint(p.x, p.y, resColor);
    }
}

void GPU::drawImage(const Image* image) {
    for (uint32_t i = 0; i < image->mWidth; ++i) {
        for (uint32_t j = 0; j < image->mHeight; ++j) {
            drawPoint(i, j, image->mData[j * image->mWidth + i]);
        }
    }
}

void GPU::drawImageWithAlpha(const Image* image, const uint32_t& alpha) {
    RGBA color;
    for (uint32_t i = 0; i < image->mWidth; ++i) {
        for (uint32_t j = 0; j < image->mHeight; ++j) {
            color = image->mData[j * image->mWidth + i];
            color.mA = alpha;
            drawPoint(i, j, color);
        }
    }
}

RGBA GPU::sampleNearest(const math::vec2f& uv) {
    auto myUV = uv;
    checkWrap(myUV.x); checkWrap(myUV.y);
    // 四舍五入到最近整数
    // u = 0 对应 x = 0，u = 1 对应 x = width - 1
    // v = 0 对应 y = 0，v = 1 对应 y = height - 1
    int x = std::round(myUV.x * (mImage->mWidth - 1));
    int y = std::round(myUV.y * (mImage->mHeight - 1));

    int position = y * mImage->mWidth + x;
    return mImage->mData[position];
}

RGBA GPU::sampleBilinear(const math::vec2f& uv) {
    RGBA resColor;
    auto myuv = uv; // 相当于拷贝了一份 
    checkWrap(myuv.x); checkWrap(myuv.y);

    float x = uv.x * static_cast<float>(mImage->mWidth - 1);
    float y = uv.y * static_cast<float>(mImage->mHeight - 1);

    int left = std::floor(x);
    int right = std::ceil(x);
    int bottom = std::floor(y);
    int top = std::ceil(y);

    float yScale = top == bottom ? 1.0 : (top - y) / (top - bottom);
    float xScale = left == right ? 1.0 : (x - left) / (right - left);

    int posLeftTop = getImagePosition(mImage, left, top);
    int posLeftBottom = getImagePosition(mImage, left, bottom);
    int posRightTop = getImagePosition(mImage, right, top);
    int posRightBottom = getImagePosition(mImage, right, bottom);

    RGBA l = Raster::lerpRGBA(mImage->mData[posLeftTop], mImage->mData[posLeftBottom], yScale);
    RGBA r = Raster::lerpRGBA(mImage->mData[posRightTop], mImage->mData[posRightBottom], yScale);
    resColor = Raster::lerpRGBA(l, r, xScale);
    return resColor;
}

void GPU::checkWrap(float& n) {
    if (n > 1.0f || n < 0.0f) {
        bool mWrap = false; // TODO mock data
        n = FRACTION(n);
        switch (mWrap) {
        case TEXTURE_WRAP_REPEAT:
            n = FRACTION(n + 1);
            break;
        case TEXTURE_WRAP_MIRROR:
            n = 1.0f - FRACTION(n + 1);
            break;
        default:
            break;
        }
    }
}
*/



