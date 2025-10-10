#pragma once
#include "../global/base.h"
#include <Windows.h>

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

private:
    BOOL createWindow(HINSTANCE hInstance);
    ATOM registerWindowClass(HINSTANCE hInstance);

private:
    static Application* mInstance;

    bool mAlive{ true };

    HINSTANCE mWindowInst;
    WCHAR     mWindowClassName[100] = L"AppWindow";
    HWND      mHwnd;

    int mWidth = 800;
    int mHeight = 600;
};
#pragma once
