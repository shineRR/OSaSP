#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

#define TIMER_SECOND 1

SIZE MIN_WINDOW_SIZE = {300,200};
int MOVE_DISTANCE = 12;
POINTFLOAT modifierDirection = {1.0, 1.0};
int RECT_SIZE = 150;
RECT rect = {0, 0, RECT_SIZE, RECT_SIZE};

void updateRectValues(RECT &rectToChange, int horizontalPx, int verticalPx) {
    rectToChange.left += horizontalPx;
    rectToChange.top += verticalPx;
    rectToChange.right += horizontalPx;
    rectToChange.bottom += verticalPx;
}

bool calculateBorders(RECT windowRect, int horizontalPx, int verticalPx) {
    RECT tmpRect = rect;
    updateRectValues(tmpRect, horizontalPx, verticalPx);
    if (tmpRect.left < windowRect.left || tmpRect.top < windowRect.top || tmpRect.right > windowRect.right ||
        tmpRect.bottom > windowRect.bottom)
        return false;
    updateRectValues(rect, horizontalPx, verticalPx);
    return true;
}

void paintObj(HWND hWnd, int horizontalPx, int verticalPx) {
    RECT windowRect;
    if(!GetClientRect(hWnd, &windowRect))
        GetLastError();
    if (!calculateBorders(windowRect, horizontalPx, verticalPx)) return;
    InvalidateRect(hWnd, nullptr, TRUE);

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    Graphics graphics(hdc);
    Image image(L"C:\\Users\\shine\\Desktop\\Dev\\OSaSP\\L1_5Sem\\Assets\\apple.png");
    Rect destRect(rect.left, rect.top, RECT_SIZE, RECT_SIZE);
    graphics.DrawImage(&image, destRect);
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    EndPaint(hWnd, &ps);
}

void iconMovement(HWND hWnd) {
    RECT windowRect;
    int xDistance = MOVE_DISTANCE * modifierDirection.x;
    int yDistance = MOVE_DISTANCE * modifierDirection.y;
    if (!GetClientRect(hWnd, &windowRect))
        GetLastError();
    if (windowRect.bottom == 0) return;
    if (rect.bottom + yDistance > windowRect.bottom && rect.right + xDistance < windowRect.right &&
            modifierDirection.y > 0) {
        modifierDirection.y *= -1.0;
    } else if (rect.bottom + yDistance > windowRect.bottom && rect.left + xDistance > windowRect.left
            && modifierDirection.y > 0) {
        modifierDirection.y *= -1.0;
    } else if (rect.right + xDistance > windowRect.right && rect.bottom + yDistance <= windowRect.bottom &&
               modifierDirection.x > 0) {
        modifierDirection.x = -1.0;
    } else if (rect.top + yDistance < windowRect.top && rect.left + xDistance > windowRect.left &&
               modifierDirection.y < 0) {
        modifierDirection.y *= -1.0;
    } else if (rect.left + xDistance < windowRect.left && rect.top + yDistance >= windowRect.top &&
               modifierDirection.x < 0) {
        modifierDirection.x *= -1.0;
    }
    paintObj(hWnd,MOVE_DISTANCE * modifierDirection.x, MOVE_DISTANCE * modifierDirection.y);
}

void updateRectPos(HWND hWnd) {
    RECT windowRect;
    if(!GetClientRect(hWnd, &windowRect))
        GetLastError();
    if (windowRect.bottom == 0) return;
    if(rect.bottom <= windowRect.bottom && rect.right > windowRect.right) {
        rect.right = windowRect.right;
        rect.left = rect.right - RECT_SIZE;
    } else if (rect.right > windowRect.right && rect.bottom > windowRect.bottom) {
        rect.right = windowRect.right;
        rect.bottom = windowRect.bottom;
        rect.top = rect.bottom - RECT_SIZE;
        rect.left = rect.right - RECT_SIZE;
    } else if(rect.right <= windowRect.right && rect.bottom > windowRect.bottom) {
        rect.bottom = windowRect.bottom;
        rect.top = rect.bottom - RECT_SIZE;
    }
    paintObj(hWnd, 0, 0);
}

void keyDownAnalyse(HWND hWnd, WPARAM wParam) {
    if (GetKeyState(VK_UP) < 0 && GetKeyState(VK_RIGHT) < 0) {
        paintObj(hWnd, MOVE_DISTANCE, -MOVE_DISTANCE);
        return;
    } else if (GetKeyState(VK_UP) < 0 && GetKeyState(VK_LEFT) < 0) {
        paintObj(hWnd, -MOVE_DISTANCE, -MOVE_DISTANCE);
        return;
    } else if (GetKeyState(VK_DOWN) < 0 && GetKeyState(VK_LEFT) < 0) {
        paintObj(hWnd, -MOVE_DISTANCE, MOVE_DISTANCE);
        return;
    } else if (GetKeyState(VK_DOWN) < 0 && GetKeyState(VK_RIGHT) < 0) {
        paintObj(hWnd, MOVE_DISTANCE, MOVE_DISTANCE);
        return;
    }
    switch (wParam) {
        case VK_UP:
            paintObj(hWnd, 0, -MOVE_DISTANCE);
            break;
        case VK_DOWN:
            paintObj(hWnd, 0, MOVE_DISTANCE);
            break;
        case VK_LEFT:
            paintObj(hWnd, -MOVE_DISTANCE, 0);
            break;
        case VK_RIGHT:
            paintObj(hWnd, MOVE_DISTANCE, 0);
            break;
    }
}

void mouseWheelAnalyse(HWND hWnd, WPARAM wParam) {
    if(GetKeyState(VK_SHIFT) < 0) {
        if(GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            paintObj(hWnd, 0, -MOVE_DISTANCE);
        else if(GET_WHEEL_DELTA_WPARAM(wParam) < 0)
            paintObj(hWnd, 0, MOVE_DISTANCE);
        return;
    }
    if(GET_WHEEL_DELTA_WPARAM(wParam) > 0)
        paintObj(hWnd, MOVE_DISTANCE, 0);
    else if(GET_WHEEL_DELTA_WPARAM(wParam) < 0)
        paintObj(hWnd, -MOVE_DISTANCE, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_CREATE:
            SetTimer(hWnd, TIMER_SECOND, 200, NULL);
            break;
        case WM_GETMINMAXINFO: {
            updateRectPos(hWnd);
            LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
            lpMMI->ptMinTrackSize.x = MIN_WINDOW_SIZE.cx;
            lpMMI->ptMinTrackSize.y = MIN_WINDOW_SIZE.cy;
        }
        case WM_TIMER:
            iconMovement(hWnd);
            break;
        case WM_KEYDOWN:
            keyDownAnalyse(hWnd, wParam);
            break;
        case WM_MOUSEWHEEL:
            mouseWheelAnalyse(hWnd, wParam);
            break;
        case WM_PAINT:
            paintObj(hWnd, 0, 0);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    WNDCLASSEX wcex; HWND hWnd; MSG msg;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 4);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = "Main";
    wcex.hIconSm = wcex.hIcon;
    RegisterClassEx(&wcex);
    hWnd = CreateWindow("Main", ":DDD;",
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
                        CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}