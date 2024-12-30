// PersistentDesktopIcons.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "PersistentDesktopIcons.h"
#include "IconManager.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

IconManager* g_icm;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    InitCommonControls();
    INITCOMMONCONTROLSEX icex{};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_USEREX_CLASSES;
    InitCommonControlsEx(&icex);

    IconManager mgr{};
    g_icm = &mgr;

    // Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_PERSISTENTDESKTOPICONS, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);

    // Perform application initialization:
    //if (!InitInstance(hInstance, nCmdShow))
    //{
    //    return FALSE;
    //}
    
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, WndProc);

    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PERSISTENTDESKTOPICONS));

    //MSG msg;

    //// Main message loop:
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}

    //g_icm = nullptr;

    //return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PERSISTENTDESKTOPICONS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PERSISTENTDESKTOPICONS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW/* & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME*/,
      CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

static void LoadFile(TCHAR* fileName = nullptr) {
    if (fileName != nullptr) {
        g_icm->Import(fileName);
        return;
    }

    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH]{};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("INI Files\0*.INI\0All Files\0*.*\0");  // Filter for INI files
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = _T("Load");
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        g_icm->Import(ofn.lpstrFile);
    }
}

static void SaveFile(TCHAR* fileName = nullptr) {
    if (fileName != nullptr) {
        g_icm->Export(fileName);
        return;
    }

    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH]{};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("INI Files\0*.INI\0All Files\0*.*\0");  // Filter for INI files
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = _T("Save");
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE)
    {
        g_icm->Export(ofn.lpstrFile);
    }
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {   
        HWND comboBox = GetDlgItem(hWnd, IDC_COMBO);
            
        ComboBox_AddString(comboBox, _T("(New)"));
        ComboBox_SetCurSel(comboBox, 0);

        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile(_T("*.ini"), &findFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                ComboBox_AddString(comboBox, findFileData.cFileName);
            } while (FindNextFile(hFind, &findFileData) != 0);
            FindClose(hFind);
        }
               
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_LOAD:
            LoadFile();
            break;
        case IDM_SAVE:
            SaveFile();
            break;
        case IDC_RESTORE:
        case IDC_SAVE:
        {
            HWND comboBox = GetDlgItem(hWnd, IDC_COMBO);
            auto sel = ComboBox_GetCurSel(comboBox);
            if (sel == 0) {
                SaveFile();
                break;
            }
            
            TCHAR text[MAX_PATH];
            ComboBox_GetText(comboBox, text, MAX_PATH);

            TCHAR path[MAX_PATH];
            _tfullpath(path, text, MAX_PATH);

            if (wmId == IDC_RESTORE) 
            {
                auto result = MessageBox(hWnd, _T("This will apply the selected profile onto your desktop."), _T("Confirm"), MB_OKCANCEL | MB_ICONWARNING);
                if (result == IDOK) {
                    LoadFile(path);
                }
            }
            else if (wmId == IDC_SAVE)
            {
                auto result = MessageBox(hWnd, _T("This will override the selected profile."), _T("Confirm"), MB_OKCANCEL | MB_ICONWARNING);
                if (result == IDOK) {
                    SaveFile(path);
                }
            }

            return TRUE;
        }
        case IDM_EXIT:
        case IDOK:
        case IDCANCEL:
            EndDialog(hWnd, wmId);
            return TRUE;
        }

        if (HIWORD(wParam) == CBN_SELCHANGE) {
            int index = ComboBox_GetCurSel((HWND)lParam);
            EnableWindow(GetDlgItem(hWnd, IDC_RESTORE), index != 0);
        }

        break;
    }
   // case WM_PAINT:
       // {
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            //// TODO: Add any drawing code that uses hdc here...
            //EndPaint(hWnd, &ps);
       // }
        //break;
    //case WM_DESTROY:
    //    PostQuitMessage(0);
    //    break;
    //default:
    //    return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
