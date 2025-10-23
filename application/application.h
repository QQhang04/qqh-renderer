#pragma once
#include "../global/base.h"
#include "camera.h"
#include <Windows.h>
#include <windowsx.h>

#define app Application::getInstance()

class Application {
public:
    static Application* getInstance();
    Application();
    ~Application();

    // 注册窗体类 创建窗体 显示窗体
    bool initApplication(HINSTANCE hInstance, const uint32_t& width = 800, const uint32_t& height = 600);
    // 托管wndProc捕获的信息，并且进行处理
    void handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    // 每一帧的循环都会调用，用来分发消息
    bool peekMessage();

    // 用于绘图
    void show();

    uint32_t getWidth() const { return mWidth; }
    uint32_t getHeight() const { return mHeight; }
    void* getCanvas() const { return mCanvasBuffer; }

    void setCamera(Camera* cam) {
        this->mCamera = cam;
    }

private:
    BOOL createWindow(HINSTANCE hInstance);
    ATOM registerWindowClass(HINSTANCE hInstance);

private:
    static Application* mInstance;

    Camera* mCamera{ nullptr };

    bool mAlive{ true };

    HINSTANCE mWindowInst;
    WCHAR     mWindowClassName[100] = L"AppWindow";
    HWND      mHwnd;

    int mWidth = 800;
    int mHeight = 600;

    HDC mhDC; // 当前窗口主dc
    HDC mCanvasDC; // 创建的于mhDC相兼容的绘图用的dc
    HBITMAP mhBmp; // mCanvasDC内部生成的bitmap
    void* mCanvasBuffer{ nullptr }; // mhBmp对应的内存起始位置指针
};
