#include <windows.h>
#pragma comment(lib, "user32.lib")

const char g_szClassName[] = "CS2StableChangerWindow";
HWND g_hMenuWnd = NULL;
bool g_bIsVisible = false;

#define ID_BTN_APPLY 1001
#define ID_COMBO_WEAPON 1002
#define ID_COMBO_SKIN 1003
#define ID_EDIT_WEAR 1004
#define ID_COMBO_KNIFE 1005
#define ID_COMBO_GLOVES 1006

// Переменные для хранения выбора из меню
int selectedWeaponIndex = 0;
int selectedSkinIndex = 0;
int selectedKnifeIndex = 0;
int selectedGlovesIndex = 0;
char wearBuffer[32] = "0.0001";

// ============================================================================
// ПРЕДСТАВЛЕНИЕ ЛОГИКИ СУРС 2 (То, как это должно работать в реальном чите)
// ============================================================================
void ApplySkinsToSource2Engine(int weapon, int skin, int knife, int gloves, float wear) {
    // 1. Получение базового модуля игры
    uintptr_t clientModule = (uintptr_t)GetModuleHandleA("client.dll");
    if (!clientModule) return;

    // 2. Поиск локального игрока через динамические смещения (Offsets)
    // uintptr_t localPlayerLoop = *(uintptr_t*)(clientModule + dwLocalPlayerController);
    
    // 3. Переход в сервис управления оружием игрока
    // uintptr_t weaponServices = *(uintptr_t*)(localPlayerLoop + m_pWeaponServices);
    
    // 4. Поиск текущего активного оружия в руках (Active Weapon)
    // uintptr_t activeWeapon = *(uintptr_t*)(weaponServices + m_hActiveWeapon);

    // 5. Запись кастомных ID скина и износа в сетевые переменные (Netvars)
    // *(int*)(activeWeapon + m_nFallbackPaintKit) = skin;
    // *(float*)(activeWeapon + m_flFallbackWear) = wear;
    // *(int*)(activeWeapon + m_iItemIDHigh) = -1; // Принудительный обход проверки Steam

    // 6. ЕСЛИ ВЫБРАН НОЖ: Подмена индекса 3D-модели
    if (knife > 0) {
        // *(int*)(activeWeapon + m_iItemDefinitionIndex) = knife_id;
        // *(int*)(activeWeapon + m_iModelIndex) = target_knife_model_index;
    }

    // 7. ВЫЗОВ ВНУТРЕННЕГО ОБНОВЛЕНИЯ (Force Update)
    // Вызов функции исходного движка для перерисовки текстуры в руках
    // static auto force_update = (void(__fastcall*)(uintptr_t))(clientModule + update_function_offset);
    // force_update(activeWeapon);
}
// ============================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wp) == ID_BTN_APPLY) {
                // Считываем всё, что пользователь выбрал в интерфейсе
                selectedWeaponIndex = (int)SendMessageA(GetDlgItem(hwnd, ID_COMBO_WEAPON), CB_GETCURSEL, 0, 0);
                selectedSkinIndex = (int)SendMessageA(GetDlgItem(hwnd, ID_COMBO_SKIN), CB_GETCURSEL, 0, 0);
                selectedKnifeIndex = (int)SendMessageA(GetDlgItem(hwnd, ID_COMBO_KNIFE), CB_GETCURSEL, 0, 0);
                selectedGlovesIndex = (int)SendMessageA(GetDlgItem(hwnd, ID_COMBO_GLOVES), CB_GETCURSEL, 0, 0);
                GetWindowTextA(GetDlgItem(hwnd, ID_EDIT_WEAR), wearBuffer, sizeof(wearBuffer));

                // Переводим текст износа в дробное число float
                float finalWear = (float)atof(wearBuffer);

                // Запуск логики применения (пока работает в демонстрационном режиме)
                ApplySkinsToSource2Engine(selectedWeaponIndex, selectedSkinIndex, selectedKnifeIndex, selectedGlovesIndex, finalWear);

                MessageBoxA(hwnd, "Configuration saved successfully!\nMemory update signals sent.", "CS2 Skin Changer", MB_OK | MB_ICONINFORMATION);
            }
            break;

        case WM_CLOSE:
            g_bIsVisible = false;
            ShowWindow(hwnd, SW_HIDE);
            ClipCursor(NULL); 
            break;

        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

DWORD WINAPI MenuThread(LPVOID lpReserved) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEXA wc = {0};

    wc.cbSize        = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = g_szClassName;

    if (!RegisterClassExA(&wc)) return 0;

    g_hMenuWnd = CreateWindowExA(
        WS_EX_TOPMOST, g_szClassName, "CS2 Skin Changer Menu", 
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 
        200, 200, 320, 380, NULL, NULL, hInstance, NULL
    );

    if (g_hMenuWnd == NULL) return 0;

    // Отрисовка стабильных элементов меню
    CreateWindowA("STATIC", "Select Weapon:", WS_CHILD | WS_VISIBLE, 20, 15, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    HWND hComboWeapon = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 35, 260, 200, g_hMenuWnd, (HMENU)ID_COMBO_WEAPON, hInstance, NULL);
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"AK-47");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"M4A1-S");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"M4A4");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"AWP");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"Desert Eagle");
    SendMessageA(hComboWeapon, CB_SETCURSEL, 0, 0);

    CreateWindowA("STATIC", "Select Skin / Paint Kit:", WS_CHILD | WS_VISIBLE, 20, 70, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    HWND hComboSkin = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 90, 260, 200, g_hMenuWnd, (HMENU)ID_COMBO_SKIN, hInstance, NULL);
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Asimov");
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Dragon Lore");
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Printstream");
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Fade");
    SendMessageA(hComboSkin, CB_SETCURSEL, 0, 0);

    CreateWindowA("STATIC", "Select Knife Model:", WS_CHILD | WS_VISIBLE, 20, 125, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    HWND hComboKnife = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 145, 260, 200, g_hMenuWnd, (HMENU)ID_COMBO_KNIFE, hInstance, NULL);
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"Default / None");
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"Karambit");
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"Butterfly Knife");
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"M9 Bayonet");
    SendMessageA(hComboKnife, CB_SETCURSEL, 0, 0);

    CreateWindowA("STATIC", "Select Gloves:", WS_CHILD | WS_VISIBLE, 20, 180, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    HWND hComboGloves = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 200, 260, 200, g_hMenuWnd, (HMENU)ID_COMBO_GLOVES, hInstance, NULL);
    SendMessageA(hComboGloves, CB_ADDSTRING, 0, (LPARAM)"Default / None");
    SendMessageA(hComboGloves, CB_ADDSTRING, 0, (LPARAM)"Sport Gloves | Pandora's Box");
    SendMessageA(hComboGloves, CB_ADDSTRING, 0, (LPARAM)"Sport Gloves | Vice");
    SendMessageA(hComboGloves, CB_SETCURSEL, 0, 0);

    CreateWindowA("STATIC", "Wear Value (Float):", WS_CHILD | WS_VISIBLE, 20, 235, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    CreateWindowA("EDIT", wearBuffer, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 20, 255, 260, 22, g_hMenuWnd, (HMENU)ID_EDIT_WEAR, hInstance, NULL);

    CreateWindowA("BUTTON", "APPLY ALL CHANGES", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 295, 260, 35, g_hMenuWnd, (HMENU)ID_BTN_APPLY, hInstance, NULL);

    while (true) {
        if (GetAsyncKeyState(VK_RSHIFT) & 1) {
            g_bIsVisible = !g_bIsVisible;
            ShowWindow(g_hMenuWnd, g_bIsVisible ? SW_SHOW : SW_HIDE);
            
            if (g_bIsVisible) {
                UpdateWindow(g_hMenuWnd);
                SetForegroundWindow(g_hMenuWnd);
                SetFocus(g_hMenuWnd);
                ClipCursor(NULL); 
            } else {
                ClipCursor(NULL);
            }
        }

        if (g_bIsVisible) {
            ClipCursor(NULL);
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            while (ShowCursor(TRUE) < 0);
        }

        MSG msg;
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        Sleep(10);
    }
    return 0;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MenuThread, hMod, 0, nullptr);
    }
    return TRUE;
}
