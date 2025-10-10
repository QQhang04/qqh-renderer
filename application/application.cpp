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

void Application::show() {
    BitBlt(mhDC, 0, 0, mWidth, mHeight, mCanvasDC, 0, 0, SRCCOPY);
}


bool Application::initApplication(HINSTANCE hInstance, const uint32_t& width, const uint32_t& height) {
    mWidth = width;
    mHeight = height;

    // ��ʼ���������ͣ�����ע��
    registerWindowClass(hInstance);

    // ����һ�����壬������ʾ
    if (!createWindow(hInstance)) {
        return false;
    }

    // ��ͼ�����Ĺ���
    mhDC = GetDC(mHwnd);
    mCanvasDC = CreateCompatibleDC(mhDC);

    BITMAPINFO bmpInfo{};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = mWidth;
    bmpInfo.bmiHeader.biHeight = mHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB; // ʵ���ϴ洢��ʽΪbgra

    mhBmp = CreateDIBSection(mCanvasDC, &bmpInfo, DIB_RGB_COLORS, (void**)&mCanvasBuffer, 0, 0);

    SelectObject(mCanvasDC, mhBmp);

    memset(mCanvasBuffer, 0, mWidth * mHeight * 4); // ���bufferΪ0

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
            DestroyWindow(hWnd); // �˴����ٴ��壬���Զ�����WM_DESTROY
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
            PostQuitMessage(0); // �����߳���ֹ����
            mAlive = false;
            break;
        }
    }
}

ATOM Application::registerWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW; // ˮƽ/��ֱ��С�����仯�ػ洰��
    wndClass.lpfnWndProc = Wndproc; // ���ûص�����
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;  // Ӧ�ó�����
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Ӧ�ó���ͼ�꣬���������Ĵ�ͼ��
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);     // ���ͼ��
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // ���ڱ���ɫ
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = mWindowClassName;  // ��������
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);     // ���ڱ���ͼ��

    return RegisterClassExW(&wndClass);
}

BOOL Application::createWindow(HINSTANCE hInstance) {
    mWindowInst = hInstance;
    auto dwExStyle = WS_EX_APPWINDOW;
    auto dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    // ���ڴ��ڱ������ȣ���Ҫ�����м���ʾ����Ĵ�С
    RECT windowRect;
    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = (long)mWidth;
    windowRect.bottom = (long)mHeight;
    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

    // ��������
    mHwnd = CreateWindowW(
        mWindowClassName,                // ��������
        (LPCWSTR)L"GraphicLearning",     // �������
        dwStyle,
        500,                            // xλ��
        500,                            // yλ��
        windowRect.right - windowRect.left,  // ���
        windowRect.bottom - windowRect.top,  // �߶�
        nullptr,                        // ������
        nullptr,                        // �˵���
        hInstance,                      // ����ʵ��
        nullptr                         // �������
    );

    if (!mHwnd) {
        return FALSE;
    }

    ShowWindow(mHwnd, true);
    UpdateWindow(mHwnd);

    return TRUE;
}