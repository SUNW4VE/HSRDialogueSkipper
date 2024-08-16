/*

Issues:

- HSR anticheat blocks GetForegroundWindow() when focused
- HSR anticheat blocks GetKeyState() and GetAsyncKeyState() when focused
- Hook implementation busted the toggle logic
- how does AHK do it?????

*/

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <thread>

const std::string version = "1.0";
const LPCSTR gameTitle = "Honkai Star Rail";

HHOOK hHook;
bool exitKey = false;
bool keypress = false;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
NOTIFYICONDATAA &loadTrayIcon();
HWND findHandle(LPCSTR title);

int main() {

    SetProcessDPIAware();
    NOTIFYICONDATAA nid = loadTrayIcon();

    std::cout << "Launched HSRDialogueSkipper ver. " << version << ".\n";

    // Determine where to click based on monitor resolution
    const int PIXEL_X = GetSystemMetrics(SM_CXSCREEN) * 0.75;
    const int PIXEL_Y = GetSystemMetrics(SM_CYSCREEN) * 0.75;

    HWND gameHandle = findHandle(gameTitle);
    std::cout << "Found \"" << gameTitle << "\".\n";
    std::cout << "\n - Toggle Skipping: [Y]\n";
    std::cout << " - Exit Program: [Right ALT]\n\n";

    // Set the hook
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (hHook == NULL) {
        std::cout << "Failed to install hook!" << std::endl;
        return 1;
    }

    MSG msg;
    bool toggled = false;
    bool wasPressed = false;
    while (exitKey == false) {

        // flag manipulation
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetForegroundWindow() == gameHandle) {
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (keypress) {
                
                if (!wasPressed) {

                    std::cout << "Switched toggle.";
                    toggled = !toggled;
                    wasPressed = true;

                }
            }
            else { 

                wasPressed = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(30));

            }

            if (toggled) {

                mouse_event(MOUSEEVENTF_LEFTDOWN, PIXEL_X, PIXEL_Y, 0, 0);
	            std::this_thread::sleep_for(std::chrono::milliseconds(30));
	            mouse_event(MOUSEEVENTF_LEFTUP, PIXEL_X, PIXEL_Y, 0, 0);

            }

        }
        else {

            toggled = false;
            wasPressed = false;

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "\nExiting HSRDialogueSkipper.\n";
    Shell_NotifyIcon(NIM_DELETE, &nid);
    UnhookWindowsHookEx(hHook);
    return 0;
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    // flag manipulation
    if (nCode >= 0) {
        // Check for keypress
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {

            KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT *)lParam;
            if (pKeyboard->vkCode == VK_RMENU) {
                // std::cout << "\nRight ALT key was pressed." << std::endl;
                exitKey = true;
            }
            if (pKeyboard->vkCode == 0x59) {
                std::cout << "\nY key was pressed." << std::endl;
                keypress = true;
            }
        }
    }
    // Pass hook data to next hook procedure
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}


NOTIFYICONDATAA &loadTrayIcon() {

    // Don't know how to make a right-click context menu for exiting the program
    static NOTIFYICONDATAA nid = {};
    AllocConsole();
    HWND dsHandle = FindWindowA("ConsoleWindowClass", NULL);

    nid.cbSize = sizeof(nid);
    nid.hWnd = dsHandle;
    nid.uFlags = NIF_ICON | NIF_TIP;
    
    nid.hIcon = (HICON)LoadImage(NULL, ".\\icon.ico", IMAGE_ICON, 0, 0, 
        LR_LOADFROMFILE | LR_SHARED);
    memcpy(nid.szTip, "HSRDialogueSkipper", 19);
    
    Shell_NotifyIcon(NIM_ADD, &nid);
    return nid;
}


HWND findHandle(LPCSTR title) {

    // find HSR window handle
    HWND gameHandle = NULL;
    std::cout << "Finding \"" << gameTitle << "\":\n";
    while (!gameHandle) {
		gameHandle = FindWindowA(0, title);
		Sleep(500);
	}

    return gameHandle;
}
