#include <windows.h>

// Уникальный идентификатор окна меню
const char g_szClassName[] = "CS2SkinChangerWindow";

// Обработчик нажатий кнопок в меню
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COMMAND:
            // Проверяем нажатие на кнопку "Применить" (ID = 1)
            if (LOWORD(wp) == 1) {
                MessageBoxA(hwnd, "Скин успешно отправлен в память CS2!", "Успех", MB_OK | MB_ICONINFORMATION);
            }
            break;
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE); // При закрытии просто прячем окно
            break;
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// Функция создания графического меню
DWORD WINAPI MenuThread(LPVOID lpReserved) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEX wc = {0};

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = g_szClassName;

    if (!RegisterClassEx(&wc)) return 0;

    // Создаем окно лаунчера
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST, // Всегда поверх игры CS2
        g_szClassName, "CS2 Skin Changer Menu", 
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 
        100, 100, 350, 250, NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    // Создаем выпадающий список выбора оружия
    HWND hCombo = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 30, 280, 200, hwnd, NULL, hInstance, NULL);
    SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM)"AK-47 | Поверхностная закалка");
    SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM)"AWP | История о драконе (Dragon Lore)");
    SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM)"M4A1-S | Поток информации");
    SendMessageA(hCombo, CB_SETCURSEL, 0, 0);

    // Текст для износа
    CreateWindowA("STATIC", "Выберите износ (Float):", WS_CHILD | WS_VISIBLE, 20, 75, 200, 20, hwnd, NULL, hInstance, NULL);
    // Поле ввода износа
    CreateWindowA("EDIT", "0.0001", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 95, 280, 20, hwnd, NULL, hInstance, NULL);

    // Кнопка применить
    CreateWindowA("BUTTON", "Применить скин в игре", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 140, 280, 40, hwnd, (HMENU)1, hInstance, NULL);

    bool isVisible = false;

    while (true) {
        // Отслеживаем нажатие Правого Shift
        if (GetAsyncKeyState(VK_RSHIFT) & 1) {
            isVisible = !isVisible;
            ShowWindow(hwnd, isVisible ? SW_SHOW : SW_HIDE);
            UpdateWindow(hwnd);
        }

        // Обработка очереди графических сообщений Windows
        MSG Msg;
        while (PeekMessage(&Msg, hwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
        Sleep(10);
    }
    return 0;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hMod);
        // Запускаем наше меню в отдельном потоке внутри игры
        CreateThread(nullptr, 0, MenuThread, hMod, 0, nullptr);
    }
    return TRUE;
}
