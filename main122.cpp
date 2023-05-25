#include <iostream>
#include <Windows.h>

HHOOK keyboardHook;
HWND overlayWindow;
bool blackScreenActive = false;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && ((KBDLLHOOKSTRUCT*)lParam)->vkCode == VK_F9)
        {
            blackScreenActive = !blackScreenActive;

            if (blackScreenActive)
            {
                WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
                wc.lpfnWndProc = DefWindowProcW;
                wc.hInstance = GetModuleHandle(NULL);
                wc.hCursor = LoadCursor(NULL, IDC_ARROW);
                wc.lpszClassName = L"OverlayClass";
                RegisterClassExW(&wc);

                overlayWindow = CreateWindowExW(
                    WS_EX_TOPMOST,
                    wc.lpszClassName,
                    NULL,
                    WS_POPUP | WS_VISIBLE,
                    0,
                    0,
                    GetSystemMetrics(SM_CXSCREEN),
                    GetSystemMetrics(SM_CYSCREEN),
                    NULL,
                    NULL,
                    wc.hInstance,
                    NULL
                );

                HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
                SetClassLongPtr(overlayWindow, GCLP_HBRBACKGROUND, (LONG_PTR)blackBrush);
                InvalidateRect(overlayWindow, NULL, TRUE);
            }
            else
            {
                DestroyWindow(overlayWindow);
            }
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);

    if (keyboardHook == NULL)
    {
        std::cout << "Failed to install hook!" << std::endl;
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    return 0;
}

