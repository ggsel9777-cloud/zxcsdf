#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>

// Базовая логика для проверки работы DLL
bool showMenu = false;

// Функция, которая выполнится при успешном внедрении в игру
DWORD WINAPI MainThread(LPVOID lpReserved) {
    // Выводим сообщение, чтобы убедиться, что лаунчер сработал
    MessageBoxA(NULL, "Скинчейнджер успешно загружен! Нажмите OK.", "CS2 Changer", MB_OK | MB_ICONINFORMATION);
    
    while (true) {
        // Проверяем нажатие Правого Shift (VK_RSHIFT)
        if (GetAsyncKeyState(VK_RSHIFT) & 1) {
            showMenu = !showMenu;
            // Звуковой сигнал при нажатии (высокий тон — включено, низкий — выключено)
            Beep(showMenu ? 800 : 500, 200); 
        }
        Sleep(10);
    }
    return 0;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hMod);
        // Создаем поток внутри игры
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
    }
    return TRUE;
}
