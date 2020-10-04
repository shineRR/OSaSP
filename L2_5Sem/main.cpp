#include <windows.h>
#include <iostream>

#define COLUMNS 5
#define ROWS 5

const SIZE MIN_WINDOW_SIZE = {150,200};
RECT rect;
int hSize, vSize;

const LPCSTR arrayText[5][5] = {"As ", "collected", "deficient", "objection", "by ",
                           {"it", "discovery", "sincerity" ,"curiosity" ,"Quiet"},
                           {"decay ", "who", "round", "three", "world"},
                           {"whole", "has", "mrs", "man", "Built"},
                           {"the", "china", "there", "tried", "jokes "}};

void drawVBorders(HDC hdc) {
    for (int i = 0; i < COLUMNS; i++) {
        MoveToEx(hdc, i * hSize, 0, NULL);
        LineTo(hdc, i * hSize, rect.bottom);
    }
}

void drawHBorder(HDC hdc, int rowOffset) {
    MoveToEx(hdc, 0, rowOffset, NULL);
    LineTo(hdc, rect.right, rowOffset);
}

void drawTable(HWND hWnd) {
    HDC hdc = GetDC(hWnd);
    vSize = (rect.bottom - rect.top) / COLUMNS;
    hSize = (rect.right - rect.left) / ROWS;
    drawVBorders(hdc);
    int offset = 2;
    int rowOffset = 0;
    for (int i = 0; i < COLUMNS; i++) {
        int maxTextHeight = vSize;
        for (int j = 0; j < COLUMNS; j++) {
            LPCSTR text = arrayText[i][j];
            RECT gridRect = {j * hSize + offset, i * vSize + rowOffset, (j + 1) * hSize, (i + 1) * vSize + rowOffset};
            int textHeight = DrawTextA(hdc, text, strlen(text), &gridRect, DT_WORDBREAK | DT_NOCLIP | DT_EDITCONTROL);
            std::cout << textHeight;
            if (textHeight > maxTextHeight)
                maxTextHeight = textHeight;
        }
        drawHBorder(hdc, (i * vSize + rowOffset));
        rowOffset += maxTextHeight - vSize;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    switch (message) {
        case WM_CREATE:
            break;
        case WM_GETMINMAXINFO: {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
            lpMMI->ptMinTrackSize.x = MIN_WINDOW_SIZE.cx;
            lpMMI->ptMinTrackSize.y = MIN_WINDOW_SIZE.cy;
            GetClientRect(hWnd, &rect);
            break;
        }
        case WM_PAINT: {
            GetClientRect(hWnd, &rect);
            BeginPaint(hWnd, &ps);
            drawTable(hWnd);
            EndPaint(hWnd, &ps);
            break;
        }
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
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = "Main";
    wcex.hIconSm = wcex.hIcon;
    RegisterClassEx(&wcex);
    hWnd = CreateWindow("Main", ":DDD;",
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                        CW_USEDEFAULT, 640, 480, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}