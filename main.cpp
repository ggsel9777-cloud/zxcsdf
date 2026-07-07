#include <windows.h>

bool showMenu = false;

DWORD WINAPI MainThread(LPVOID lpReserved) {
    MessageBoxA(NULL, "Successfully loaded! Press OK.", "CS2 Changer", MB_OK | MB_ICONINFORMATION);
    
    while (true) {
        if (GetAsyncKeyState(VK_RSHIFT) & 1) {
            showMenu = !showMenu;
            Beep(showMenu ? 800 : 500, 200); 
        }
        Sleep(10);
    }
    return 0;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
    }
    return TRUE;
}
