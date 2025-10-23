#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

// 相机
Camera* cam = nullptr;
math::vec3f camPos;

// 纹理
Image* image;
uint32_t texture;

//三个属性对应vbo
uint32_t positionVbo = 0;
uint32_t positionVbo1 = 0;
uint32_t colorVbo = 0;
uint32_t colorVbo1 = 0;
uint32_t uvVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//三角形专属vao
uint32_t vao = 0;
uint32_t vao1 = 0;

// 使用的Shader
TextureShader* shader = nullptr;

//mvp变换矩阵
math::Mat4f modelMatrix;
math::Mat4f viewMatrix;
math::Mat4f perspectiveMatrix;

float angle = 0.0f;
void transform() {
    angle += 0.01f;
    //模型变换
    modelMatrix = math::rotate(math::Mat4f(1.0f), angle, math::vec3f(0.0f, 1.0f, 0.0f));
}

void prepare() {
    camPos = { 0, 0, 3 };
    cam = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, math::vec3f(0, 1, 0), camPos);
    app->setCamera(cam);

    image = Image::createImage("assets/textures/pika.png");
    texture = sgl->genTexture();
    sgl->bindTexture(texture);
    sgl->texImage2D(image->mWidth, image->mHeight, image->mData);
    sgl->texParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
    sgl->texParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
    sgl->texParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
    sgl->bindTexture(0);

    float positions[] = {
        -0.5f, -0.288675f, 0.0f,  // 左下
         0.5f, -0.288675f, 0.0f,  // 右下
         0.0f,  0.57735f,  0.0f,  // 顶点
    };

    float colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
    };

    float uvs[] = {
        0.0f, 0.0f,   // 对应左下角
        1.0f, 0.0f,   // 对应右下角
        0.5f, 1.0f,   // 顶部中心
    };

    uint32_t indices[] = { 0, 1, 2 };

    //生成indices对应ebo
    ebo = sgl->genBuffer();
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

    //生成vao并且绑定
    vao = sgl->genVertexArray();
    sgl->bindVertexArray(vao);

    //生成每个vbo，绑定后，设置属性ID及读取参数
    auto positionVbo = sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, positionVbo);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 9, positions);
    sgl->vertexAttributePointer(0, 3, 3 * sizeof(float), 0);

    auto colorVbo = sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, colorVbo);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 12, colors);
    sgl->vertexAttributePointer(1, 4, 4 * sizeof(float), 0);

    auto uvVbo = sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
    sgl->vertexAttributePointer(2, 2, 2 * sizeof(float), 0);

    sgl->bindBuffer(ARRAY_BUFFER, 0);
    sgl->bindVertexArray(0);

    shader = new TextureShader();
    perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    auto cameraModelMatrix = math::translate(math::Mat4f(1.0f), math::vec3f(0.0f, 0.0f, 3.0f));
    viewMatrix = math::inverse(cameraModelMatrix);
}

void render() {
    transform();
    shader->mModelMatrix = modelMatrix;
    shader->mViewMatrix = cam->getViewMatrix();
    shader->mProjectionMatrix = cam->getProjectionMatrix();
    shader->mDiffuseTexture = texture;

    sgl->clear();
    sgl->useProgram(shader);

    sgl->bindVertexArray(vao);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->drawElement(DRAW_TRIANGLES, 0, 3); 
}

int APIENTRY wWinMain(
    _In_        HINSTANCE hInstance,        // 本应用程序实例句柄，唯一指代当前程序
    _In_opt_    HINSTANCE hPrevInstance,    // 本程序前一个实例，一般是null
    _In_        LPWSTR    lpCmdLine,        // 应用程序运行参数
    _In_        int       nCmdShow          // 窗口如何显示（最大化、最小化、隐藏），不需理会
)
{
    if (!app->initApplication(hInstance, 800, 600)) {
        return -1;
    }

    sgl->initSurface(app->getWidth(), app->getHeight(), app->getCanvas());

    bool alive = true;
    prepare();
    while (alive) {
        alive = app->peekMessage();
        cam->update();
        render();
        app->show();
    }

    Image::destroyImage(image);
    sgl->deleteTexture(texture);
    delete cam;
    return 0;
}