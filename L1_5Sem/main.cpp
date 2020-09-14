#include <windows.h>
#include <iostream>

SIZE MIN_WINDOW_SIZE = {300,200};
int MOVE_DISTANCE = 10;
int RECT_SIZE = 50;
RECT rect = {0, 0, RECT_SIZE, RECT_SIZE};

void updateRectValues(RECT &rectToChange, int horizontalPx, int verticalPx) {
    rectToChange.left += horizontalPx;
    rectToChange.top += verticalPx;
    rectToChange.right += horizontalPx;
    rectToChange.bottom += verticalPx;
}

bool calculateBorders(RECT windowRect, int horizontalPx, int verticalPx) {
    RECT tmpRect = rect;
    std::cout << tmpRect.bottom << " before\n";
    updateRectValues(tmpRect, horizontalPx, verticalPx);
    std::cout << tmpRect.bottom << " after\n";
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
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(hdc, RGB(200,100,0));
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    EndPaint(hWnd, &ps);
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
    switch (message)
    {
        case WM_GETMINMAXINFO:
        {
            updateRectPos(hWnd);
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = MIN_WINDOW_SIZE.cx;
            lpMMI->ptMinTrackSize.y = MIN_WINDOW_SIZE.cy;
        }
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
    return (int)msg.wParam;
}