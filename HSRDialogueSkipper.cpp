/*

Issues:

- HSR anticheat blocks GetForegroundWindow() when focused
- HSR anticheat blocks GetKeyState() and GetAsyncKeyState() when focused
- Need to add right click close menu in system tray

*/

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <thread>

NOTIFYICONDATAA &loadTrayIcon();
HWND findHandle(LPCSTR title);

const std::string version = "1.0";
const LPCSTR gameTitle = "Honkai: Star Rail";

int main() {

    SetProcessDPIAware();
    NOTIFYICONDATAA nid = loadTrayIcon();

    std::cout << "Launched HSRDialogueSkipper ver. " << version << ".\n";
    std::cout << " - Toggle Skipping: [Y]\n";
    std::cout << " - Exit Program: [ALT+Q]\n";

    // Determine where to click based on monitor resolution
    const int PIXEL_X = GetSystemMetrics(SM_CXSCREEN) * 0.75;
    const int PIXEL_Y = GetSystemMetrics(SM_CYSCREEN) * 0.75;

    HWND gameHandle = findHandle(gameTitle);
    std::cout << "Found \"" << gameTitle << "\".\n";

    bool exitKey = false;
    bool toggled = false;
    bool wasPressed = false;
    while (exitKey == false) {

        if ((GetKeyState(VK_MENU) & 0x8000) 
                && (GetKeyState('Q') & 0x8000)) {
                    exitKey = true;
                    break;
                }

        if (GetForegroundWindow() == gameHandle) {
            
            std::cout << "Focused. "; // debug
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (GetKeyState('Y') & 0x8000) {
                
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

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        toggled = false;
        wasPressed = false;
    }

    std::cout << "\nExiting HSRDialogueSkipper.\n";
    Shell_NotifyIcon(NIM_DELETE, &nid);
    return 0;
}


NOTIFYICONDATAA &loadTrayIcon() {

    // load system tray icon
    // how to make a right-click context menu for exiting the program?
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
    std::cout << "\nFinding \"" << gameTitle << "\":\n";
    while (!gameHandle) {
		gameHandle = FindWindowA(0, title);
		Sleep(500);
	}

    return gameHandle;
}
