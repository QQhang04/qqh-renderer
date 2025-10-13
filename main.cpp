#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")

Image* texture;
Point p1;
Point p2;
Point p3;

Point p11;
Point p22;
Point p33;

void prepare() {
    texture = Image::createImage("assets/textures/small.png");

    p1.x = 0;
    p1.y = 0;
    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.x = 150;
    p2.y = 300;
    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(0.5f, 1.0f);

    p3.x = 300;
    p3.y = 0;
    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 0.0f);

    p11.x = 300;
    p11.y = 0;
    p11.color = RGBA(255, 0, 0, 255);
    p11.uv = math::vec2f(0.0f, 0.0f);

    p22.x = 450;
    p22.y = 300;
    p22.color = RGBA(0, 255, 0, 255);
    p22.uv = math::vec2f(0.5f, 1.0f);

    p33.x = 600;
    p33.y = 0;
    p33.color = RGBA(0, 0, 255, 255);
    p33.uv = math::vec2f(1.0f, 0.0f);
}
 
void render() {
    sgl->clear();
    sgl->setBilinear(false);
    sgl->setTexture(texture);
    sgl->drawTriangle(p1, p2, p3);
    //sgl->setBlending(true);
    sgl->setBilinear(true);
    sgl->drawTriangle(p11, p22, p33);
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
        render();
        app->show();
    }

    return 0;
}