// PersistentDesktopIcons.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "PersistentDesktopIcons.h"
#include "IconManager.h"

// Global Variables:
HINSTANCE hInst;                                // current instance
IconManager* g_icm;
IProgressDialog* pProgressDialog;

// Forward declarations of functions included in this code module:
INT_PTR CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int __stdcall wWinMainCRTStartup()
{
#if defined(_WIN32)
    typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

    LPFN_ISWOW64PROCESS isWow64;
    BOOL bIsWow64 = FALSE;

    isWow64 = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
    if (isWow64 != NULL && isWow64(GetCurrentProcess(), &bIsWow64) && bIsWow64)
    {
        MessageBox(NULL, _T("This application only works when executed in the native architecture! Please use the 64bit version."), _T("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }
#endif

    return wWinMain(GetModuleHandle(NULL), 0, 0, 0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    IconManager mgr{};
    g_icm = &mgr;

    hInst = hInstance;
    return (int) DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, WndProc);
}

static void LoadFile(TCHAR* fileName, int offsetX = 0, int offsetY = 0, bool alignAfter = false) {
    if (pProgressDialog)
    {
        pProgressDialog->SetTitle(_T("Moving Desktop icons"));
        pProgressDialog->SetLine(1, _T("Processing..."), false, NULL);
        pProgressDialog->StartProgressDialog(NULL, NULL, PROGDLG_AUTOTIME, NULL);
    }

    if (fileName != nullptr && !g_icm->Import(fileName, pProgressDialog, offsetX, offsetY, alignAfter))
    {
        MessageBox(NULL, _T("The selected file could not be applied!"), _T("Error"), MB_OK | MB_ICONERROR);
    }

    if (pProgressDialog)
    {
        pProgressDialog->StopProgressDialog();
        pProgressDialog->Release();
        
        // Create new instance for next operation
        auto _ = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pProgressDialog));
    }
}

static void SaveFile(TCHAR* fileName) {
    if (fileName != nullptr) {
        if (!g_icm->Export(fileName)) {
            MessageBox(NULL, _T("An error occured while exporting the current file!"), _T("Error"), MB_OK | MB_ICONERROR);
        }
        return;
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
INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static TCHAR buffer[MAX_PATH + 1];

    switch (message)
    {
    case WM_CTLCOLORSTATIC:
    {
        return (INT_PTR)COLOR_WINDOWFRAME;
    }
    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        RECT rect;
        GetClientRect(hWnd, &rect);

        // Draw the upper part (white)
        FillRect(hdc, &rect, (HBRUSH)COLOR_WINDOWFRAME);

        //// Draw the lower part (grey)
        rect.top = rect.bottom - 38;
        FillRect(hdc, &rect, (HBRUSH)COLOR_WINDOW);

        return TRUE;
    }
    case WM_INITDIALOG:
    {   
        auto hr = CoInitialize(NULL);
        hr |= CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pProgressDialog));
        if (FAILED(hr))
        {
            CoUninitialize();
        }

        auto hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON));
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

        HWND comboBox = GetDlgItem(hWnd, IDC_COMBO);

        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile(_T("*.desktop"), &findFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                ComboBox_AddString(comboBox, findFileData.cFileName);
            } while (FindNextFile(hFind, &findFileData) != NULL);
            FindClose(hFind);

            ComboBox_SetCurSel(comboBox, 0);
        }
        
        return TRUE;
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
        case IDM_SAVE: {
            OPENFILENAME ofn;

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = buffer;
            ofn.nMaxFile = sizeof(buffer);
            ofn.lpstrFilter = _T("Desktop Files\0*.desktop\0All Files\0*.*\0");  // Filter for INI files
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.lpstrTitle = _T("Load from");
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (wmId == IDM_SAVE) 
            {
                ofn.lpstrTitle = _T("Save to");
                ofn.Flags |= OFN_OVERWRITEPROMPT;
            }

            if (GetSaveFileName(&ofn) == TRUE)
            {
                if (wmId == IDM_SAVE)
                    SaveFile(ofn.lpstrFile);
                else
                    LoadFile(ofn.lpstrFile);
            }

            break;
        } 
        case IDC_RESTORE:
        case IDC_SAVE:
        {
            HWND comboBox = GetDlgItem(hWnd, IDC_COMBO);
            ComboBox_GetText(comboBox, buffer, MAX_PATH);
            auto absPath = _tfullpath(NULL, buffer, MAX_PATH);

            if (wmId == IDC_RESTORE) 
            {
                HWND editOffsetX = GetDlgItem(hWnd, IDC_EDIT_OFFX);
                Edit_GetText(editOffsetX, buffer, MAX_PATH);
                auto offsetX = _tstoi(buffer);

                HWND editOffsetY = GetDlgItem(hWnd, IDC_EDIT_OFFY);
                Edit_GetText(editOffsetY, buffer, MAX_PATH);
                auto offsetY = _tstoi(buffer);

                HWND buttonAutoAlign = GetDlgItem(hWnd, IDC_ALIGN_AFTER);
                auto autoAlign = Button_GetState(buttonAutoAlign);

                auto result = MessageBox(hWnd, _T("This will apply the selected profile onto your desktop."), _T("Confirm"), MB_OKCANCEL | MB_ICONWARNING);
                if (result == IDOK) {
                    LoadFile(absPath, offsetX, offsetY, autoAlign == 1);
                }
            }
            else if (wmId == IDC_SAVE)
            {
                auto result = MessageBox(hWnd, _T("This will override the selected profile."), _T("Confirm"), MB_OKCANCEL | MB_ICONWARNING);
                if (result == IDOK) {
                    SaveFile(absPath);
                }
            }

            return TRUE;
        }
        case IDM_EXIT:
        case IDOK:
        case IDCANCEL:
            CoUninitialize();
            EndDialog(hWnd, wmId);
            return TRUE;
        }

        break;
    }
    }
    return FALSE;
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
