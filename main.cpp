#include <windows.h>
#pragma comment(lib, "user32.lib")

const char g_szClassName[] = "CS2StableChangerWindow";
HWND g_hMenuWnd = NULL;
bool g_bIsVisible = false;

// Уникальные идентификаторы для новых элементов интерфейса
#define ID_BTN_APPLY 1001
#define ID_COMBO_WEAPON 1002
#define ID_COMBO_SKIN 1003
#define ID_EDIT_WEAR 1004
#define ID_COMBO_KNIFE 1005
#define ID_COMBO_GLOVES 1006

int selectedWeaponIndex = 0;
int selectedSkinIndex = 0;
int selectedKnifeIndex = 0;
int selectedGlovesIndex = 0;
char wearBuffer[32] = "0.0001";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wp) == ID_BTN_APPLY) {
                // Сохраняем выбор пользователя из всех списков
                selectedWeaponIndex = (int)SendMessageA(GetDlgItem(hwnd, ID_COMBO_WEAPON), CB_GETCURSEL, 0, 0);
                selectedSkinIndex = (int)SendMessageA(GetDlgItem(hwnd, ID_COMBO_SKIN), CB_GETCURSEL, 0, 0);
                selectedKnifeIndex = (int)SendMessageA(GetDlgItem(hwnd, ID_COMBO_KNIFE), CB_GETCURSEL, 0, 0);
                selectedGlovesIndex = (int)SendMessageA(GetDlgItem(hwnd, ID_COMBO_GLOVES), CB_GETCURSEL, 0, 0);
                GetWindowTextA(GetDlgItem(hwnd, ID_EDIT_WEAR), wearBuffer, sizeof(wearBuffer));

                // Техническая справка по памяти CS2 (Source 2):
                // Для применения перчаток и ножей код должен изменять не только m_iItemDefinitionIndex, 
                // но и принудительно подменять индекс модели (m_pWeaponServices -> m_hMyWeapons -> m_iModelIndex)
                // на соответствующий ID кастомной модели ножа/перчаток из ресурсов игры.

                MessageBoxA(hwnd, "Skin, Knife and Gloves data saved! Applying...", "CS2 Changer", MB_OK | MB_ICONINFORMATION);
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

    // Немного увеличили высоту окна (до 380), чтобы поместились новые пункты
    g_hMenuWnd = CreateWindowExA(
        WS_EX_TOPMOST, g_szClassName, "CS2 Skin Changer Menu", 
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 
        200, 200, 320, 380, NULL, NULL, hInstance, NULL
    );

    if (g_hMenuWnd == NULL) return 0;

    // 1. Выбор оружия
    CreateWindowA("STATIC", "Select Weapon:", WS_CHILD | WS_VISIBLE, 20, 15, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    HWND hComboWeapon = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 35, 260, 200, g_hMenuWnd, (HMENU)ID_COMBO_WEAPON, hInstance, NULL);
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"AK-47");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"M4A1-S");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"M4A4");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"AWP");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"Desert Eagle");
    SendMessageA(hComboWeapon, CB_ADDSTRING, 0, (LPARAM)"USP-S");
    SendMessageA(hComboWeapon, CB_SETCURSEL, 0, 0);

    // 2. Выбор скина
    CreateWindowA("STATIC", "Select Skin / Paint Kit:", WS_CHILD | WS_VISIBLE, 20, 70, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    HWND hComboSkin = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 90, 260, 200, g_hMenuWnd, (HMENU)ID_COMBO_SKIN, hInstance, NULL);
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Asimov");
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Dragon Lore");
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Printstream");
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Fade");
    SendMessageA(hComboSkin, CB_ADDSTRING, 0, (LPARAM)"Howl");
    SendMessageA(hComboSkin, CB_SETCURSEL, 0, 0);

    // 3. НОВЫЙ БЛОК: Выбор Ножа
    CreateWindowA("STATIC", "Select Knife Model:", WS_CHILD | WS_VISIBLE, 20, 125, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    HWND hComboKnife = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 145, 260, 200, g_hMenuWnd, (HMENU)ID_COMBO_KNIFE, hInstance, NULL);
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"Default / None");
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"Karambit");
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"Butterfly Knife");
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"M9 Bayonet");
    SendMessageA(hComboKnife, CB_ADDSTRING, 0, (LPARAM)"Skeleton Knife");
    SendMessageA(hComboKnife, CB_SETCURSEL, 0, 0);

    // 4. НОВЫЙ БЛОК: Выбор Перчаток
    CreateWindowA("STATIC", "Select Gloves:", WS_CHILD | WS_VISIBLE, 20, 180, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    HWND hComboGloves = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 20, 200, 260, 200, g_hMenuWnd, (HMENU)ID_COMBO_GLOVES, hInstance, NULL);
    SendMessageA(hComboGloves, CB_ADDSTRING, 0, (LPARAM)"Default / None");
    SendMessageA(hComboGloves, CB_ADDSTRING, 0, (LPARAM)"Sport Gloves | Pandora's Box");
    SendMessageA(hComboGloves, CB_ADDSTRING, 0, (LPARAM)"Sport Gloves | Vice");
    SendMessageA(hComboGloves, CB_ADDSTRING, 0, (LPARAM)"Driver Gloves | Crimson Weave");
    SendMessageA(hComboGloves, CB_ADDSTRING, 0, (LPARAM)"Specialist Gloves | Crimson Kimono");
    SendMessageA(hComboGloves, CB_SETCURSEL, 0, 0);

    // 5. Износ
    CreateWindowA("STATIC", "Wear Value (Float):", WS_CHILD | WS_VISIBLE, 20, 235, 200, 18, g_hMenuWnd, NULL, hInstance, NULL);
    CreateWindowA("EDIT", wearBuffer, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 20, 255, 260, 22, g_hMenuWnd, (HMENU)ID_EDIT_WEAR, hInstance, NULL);

    // 6. Кнопка применения
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
