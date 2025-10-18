#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

void transform() {
   
}

uint32_t vbo = 0;
uint32_t vao = 0;

void prepare() {
    vbo = sgl->genBuffer();
    sgl->deleteBuffer(vbo);

    vao = sgl->genVertexArray();
    sgl->deleteVertexArray(vao);
}

void render() {
    transform();
    sgl->clear();
    // sgl->setBilinear(false);
    // sgl->setTexture(texture);
    //sgl->drawTriangle(p1, p2, p3);
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
    prepare();
    while (alive) {
        alive = app->peekMessage();
        render();
        app->show();
    }

    return 0;
}