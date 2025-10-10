#include "application.h"

Application* Application::mInstance = nullptr;

Application* Application::getInstance() {
    if (mInstance == nullptr) {
        mInstance = new Application();
    }
    return mInstance;
}

Application::Application() {}
Application::~Application() {}

LRESULT CALLBACK Wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    Application::getInstance()->handleMessage(hWnd, message, wParam, lParam);
    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool Application::initApplication(HINSTANCE hInstance, const uint32_t& width, const uint32_t& height) {
    mWidth = width;
    mHeight = height;

    // 初始化窗体类型，并且注册
    registerWindowClass(hInstance);

    // 生成一个窗体，并且显示
    if (!createWindow(hInstance)) {
        return false;
    }

    return true;
}

bool Application::peekMessage() {
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return mAlive;
}

void Application::handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
        case WM_CLOSE: {
            DestroyWindow(hWnd); // 此处销毁窗体，会自动发出WM_DESTROY
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_DESTROY: {
            PostQuitMessage(0); // 发出线程终止请求
            mAlive = false;
            break;
        }
    }
}

ATOM Application::registerWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW; // 水平/垂直大小发生变化重绘窗口
    wndClass.lpfnWndProc = Wndproc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;  // 应用程序句柄
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 应用程序图标，即任务栏的大图标
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);     // 鼠标图标
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // 窗口背景色
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = mWindowClassName;  // 窗口类名
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);     // 窗口标题图标

    return RegisterClassExW(&wndClass);
}

BOOL Application::createWindow(HINSTANCE hInstance) {
    mWindowInst = hInstance;
    auto dwExStyle = WS_EX_APPWINDOW;
    auto dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    // 由于存在标题栏等，需要计算中间显示区域的大小
    RECT windowRect;
    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = (long)mWidth;
    windowRect.bottom = (long)mHeight;
    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

    // 创建窗口
    mHwnd = CreateWindowW(
        mWindowClassName,                // 窗口类名
        (LPCWSTR)L"GraphicLearning",     // 窗体标题
        dwStyle,
        500,                            // x位置
        500,                            // y位置
        windowRect.right - windowRect.left,  // 宽度
        windowRect.bottom - windowRect.top,  // 高度
        nullptr,                        // 父窗口
        nullptr,                        // 菜单栏
        hInstance,                      // 程序实例
        nullptr                         // 额外参数
    );

    if (!mHwnd) {
        return FALSE;
    }

    ShowWindow(mHwnd, true);
    UpdateWindow(mHwnd);

    return TRUE;
}