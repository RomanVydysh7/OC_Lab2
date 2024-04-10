#include <windows.h>
#include <string>
#include <vector>

HINSTANCE hInst;
HWND mainWnd;
HWND hEdit;

std::vector<HWND> windows;

#define ID_FILE_NEW_WINDOW  1001
#define ID_FILE_NEW_DIALOG_WINDOW  1002
#define ID_FILE_EXIT 1003
#define ID_SHOW_WINDOW_LIST 1004

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CreateNewWindow(LPCTSTR lpWindowClass, LPCTSTR lpWindowName, DWORD dwStyle) {
    HWND newWnd = CreateWindow(
        lpWindowClass,
        lpWindowName,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInst, NULL);

    if (newWnd) {
        windows.push_back(newWnd);
        ShowWindow(newWnd, SW_SHOWNORMAL);
        UpdateWindow(newWnd);
    }
}

void ShowWindowList() {
    std::wstring list;
    list += L"Список вікон:\n\n";

    EnumWindows([](HWND hWnd, LPARAM lParam) {
        wchar_t title[256];
        GetWindowText(hWnd, title, 256);

        wchar_t class_name[256];
        GetClassName(hWnd, class_name, 256);

        wchar_t main_class_name[256];
        GetClassName(mainWnd, main_class_name, 256);

        if (std::wstring(class_name) == std::wstring(main_class_name)) {
            RECT rect;
            GetWindowRect(hWnd, &rect);

            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            int x = rect.left;
            int y = rect.top;

            wchar_t windowInfo[512];
            swprintf(windowInfo, 512, L"Назва: %s\nРозмір: %dx%d\nПоложення: (%d, %d)\n\n", title, width, height, x, y);

            *((std::wstring*)lParam) += windowInfo;
        }

        return TRUE;
        }, (LPARAM)&list);

    MessageBox(NULL, list.c_str(), L"Список вікон", MB_OK | MB_ICONINFORMATION);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"MainWinClass";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, L"Помилка під час реєстрації класу вікна.", L"Помилка", MB_ICONERROR);
        return 1;
    }

    hInst = hInstance;

    mainWnd = CreateWindow(
        L"MainWinClass",
        L"Менеджер вікон",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 700,
        NULL, NULL, hInstance, NULL);

    if (!mainWnd) {
        MessageBox(NULL, L"Помилка під час створення головного вікна.", L"Помилка", MB_ICONERROR);
        return 1;
    }

    HMENU hMenu = CreateMenu();
    HMENU hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, ID_FILE_NEW_WINDOW, L"Створити нове вікно");
    AppendMenu(hSubMenu, MF_STRING, ID_FILE_NEW_DIALOG_WINDOW, L"Створити нове діалогове вікно");
    AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, L"Вийти");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, L"Меню ");
    AppendMenu(hMenu, MF_STRING, ID_SHOW_WINDOW_LIST, L"Показати список вікон ");

    SetMenu(mainWnd, hMenu);


    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
        50, 50, 400, 200, mainWnd, NULL, hInstance, NULL);
    if (hEdit == NULL) {
        MessageBox(mainWnd, L"Не вдалося створити текстове поле.", L"Помилка", MB_OK | MB_ICONERROR);
    }

    ShowWindow(mainWnd, nCmdShow);
    UpdateWindow(mainWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FILE_NEW_WINDOW:
            CreateNewWindow(L"MainWinClass", L"Нове вікно", WS_OVERLAPPEDWINDOW);
            break;
        case ID_FILE_NEW_DIALOG_WINDOW:
            CreateNewWindow(L"#32770", L"Нове діалогове вікно", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CAPTION | DS_MODALFRAME);
            break;
        case ID_FILE_EXIT:
            PostQuitMessage(0);
            break;
        case ID_SHOW_WINDOW_LIST:
            ShowWindowList();
            break;
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(hdc, hPen);
        SelectObject(hdc, hBrush);

        Rectangle(hdc, 550, 50, 650, 150);


        DeleteObject(hPen);
        DeleteObject(hBrush);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}