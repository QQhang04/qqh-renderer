#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")

Image* image01 = Image::createImage("assets/textures/test.png");
Image* image02 = Image::createImage("assets/textures/pika.png");

void render() {
    sgl->clear();
    sgl->setBlending(true);
    sgl->drawImage(image01);
    sgl->drawImage(image02);

    /*for (uint32_t i = 0; i < app->getWidth(); ++i) {
        sgl->drawPoint(i, 300, RGBA(255, 255, 255));
    }*/
    
    /*Point a{ 0, 100, RGBA(255, 0, 0, 255) };
    Point b{ 500, 100, RGBA(0, 255, 0, 255) };
    Point c{ 250, 500, RGBA(0, 0, 255, 255) };
    sgl->drawTriangle(a, b, c);*/

    /*Point c{ 400, 300, RGBA(255, 0, 0, 255) };
    int r = 100;
    for (float i = 0; i < 360; i += 10)
    {
        float radian = DEG2RAD(i);
        int x = r * sin(radian) + c.x;
        int y = r * cos(radian) + c.y;
        Point pt{ x, y, RGBA(rand() % 255, rand() % 255, rand() % 255, 255)};
        sgl->drawLine(c, pt);
    }*/
}

int APIENTRY wWinMain(
    _In_        HINSTANCE hInstance,        // ��Ӧ�ó���ʵ�������Ψһָ����ǰ����
    _In_opt_    HINSTANCE hPrevInstance,    // ������ǰһ��ʵ����һ����null
    _In_        LPWSTR    lpCmdLine,        // Ӧ�ó������в���
    _In_        int       nCmdShow          // ���������ʾ����󻯡���С�������أ����������
)
{
    if (!app->initApplication(hInstance, 800, 600)) {
        return -1;
    }

    sgl->initSurface(app->getWidth(), app->getHeight(), app->getCanvas());

    bool alive = true;
    while (alive) {
        alive = app->peekMessage();
        render();
        app->show();
    }

    return 0;
}