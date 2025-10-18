#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Image* texture;
Point p1;
Point p2;
Point p3;

math::vec4f pos1;
math::vec4f pos2;
math::vec4f pos3;

math::Mat4f modelMatrix;
math::Mat4f viewMatrix;
math::Mat4f perspectiveMatrix;
math::Mat4f screenMatrix;

float angle = 0.0f;
float cameraPos = 5.0f;

void transform() {
    angle += 0.01f;
    //cameraPos += 0.01f;

    //ģ�ͱ任
    modelMatrix = math::rotate(math::Mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });

    //ģ�������������
    auto cameraModelMatrix = math::translate(math::Mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, cameraPos });
    viewMatrix = math::inverse(cameraModelMatrix);

    //mvp�������
    auto sp1 = perspectiveMatrix * viewMatrix * modelMatrix * pos1;
    auto sp2 = perspectiveMatrix * viewMatrix * modelMatrix * pos2;
    auto sp3 = perspectiveMatrix * viewMatrix * modelMatrix * pos3;

    //͸�ӳ������˴��������z��=0��
    sp1 /= sp1.w;
    sp2 /= sp2.w;
    sp3 /= sp3.w;

    //��Ļ�ռ�
    sp1 = screenMatrix * sp1;
    sp2 = screenMatrix * sp2;
    sp3 = screenMatrix * sp3;

    p1.x = sp1.x;
    p1.y = sp1.y;
    p2.x = sp2.x;
    p2.y = sp2.y;
    p3.x = sp3.x;
    p3.y = sp3.y;
}

void prepare() {
    texture = Image::createImage("assets/textures/test.jpg");

    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(1.0f, 1.0f);

    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 0.0f);

    pos1 = { -1.5f, 0.0f, 0.0f, 1.0f };
    pos2 = { 1.5f, 0.0f, 0.0f, 1.0f };
    pos3 = { 0.0f, 2.0f, 0.0f, 1.0f };

    perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    screenMatrix = math::screenMatrix<float>(WIDTH, HEIGHT);
}
 
void render() {
    transform();
    sgl->clear();
    // sgl->setBilinear(false);
    // sgl->setTexture(texture);
    sgl->drawTriangle(p1, p2, p3);
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