#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "application/model.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

float angle = 0;

// 光照信息
DirectionalLight directionalLight;
math::vec3f envLight;

// 相机
Camera* camera = nullptr;
math::vec3f camPos;

// 使用的Shader
LightShader* shader = nullptr;

// 使用的模型
Model* model = nullptr;

//mvp变换矩阵
math::Mat4f modelMatrix;
math::Mat4f viewMatrix;
math::Mat4f perspectiveMatrix;

void transform() {
    angle -= 0.02f;
    auto rotateMatrix = math::rotate(math::Mat4f(), angle, {0.0f, 1.0f, 0.0f});
    auto translateMatrix = math::translate(math::Mat4f(), 0.0f, 0.0f, -5.0f);
    auto m = translateMatrix * rotateMatrix;
    m = math::scale(m, 0.1f, 0.1f, 0.1f);
    model->setModelMatrix(m);
}

void prepare() {
    camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f, { 0.0f, 1.0f, 0.0f }, camPos);
    app->setCamera(camera);

    shader = new LightShader();
    directionalLight.color = {0.8f, 0.8f, 1.0f};
    directionalLight.direction = {-1.0f, -0.5f, -0.7f};
    envLight = { 0.2f, 0.2f, 0.2f };

    sgl->setEnable(CULL_FACE, true);

    model = new Model();
    model->read("assets/model/dinosaur/source/Rampaging T-Rex.glb");
    //model->read("assets/model/Fist Fight B.fbx");
    //model->read("assets/model/bag/backpack.obj");
}

void render() {
    transform();
    shader->mViewMatrix = camera->getViewMatrix();
    shader->mProjectionMatrix = camera->getProjectionMatrix();

    shader->mDirectionalLight = directionalLight;
    shader->mEnvLight = envLight;

    sgl->clear();
    sgl->useProgram(shader);

    model->draw((shader));
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
        camera->update();
        render();
        app->show();
    }

    delete camera;
    return 0;
}