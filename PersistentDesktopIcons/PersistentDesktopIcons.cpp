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

    isWow64 = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
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

static void LoadFile(TCHAR* fileName = nullptr) {
    if (pProgressDialog)
    {
        pProgressDialog->StartProgressDialog(NULL, NULL, PROGDLG_NORMAL, NULL);
        pProgressDialog->SetTitle(_T("Moving Desktop icons"));
        pProgressDialog->SetLine(1, _T("Processing..."), false, NULL);
    }

    if (fileName != nullptr && !g_icm->Import(fileName, pProgressDialog))
    {
        MessageBox(NULL, _T("The selected file could not be applied!"), _T("Error"), MB_OK | MB_ICONERROR);
    }
    else if (fileName == nullptr)
    {
        OPENFILENAME ofn;
        TCHAR szFile[MAX_PATH]{};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = _T("Desktop Files\0*.desktop\0All Files\0*.*\0");  // Filter for INI files
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.lpstrTitle = _T("Load");
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE && !g_icm->Import(ofn.lpstrFile, pProgressDialog))
        {
            MessageBox(NULL, _T("The selected file could not be applied!"), _T("Error"), MB_OK | MB_ICONERROR);
        }
    }

    if (pProgressDialog)
    {
        pProgressDialog->StopProgressDialog();
        pProgressDialog->Release();
        
        // Create new instance for next operation
        auto _ = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pProgressDialog));
    }
}

static void SaveFile(TCHAR* fileName = nullptr) {
    if (fileName != nullptr) {
        if (!g_icm->Export(fileName)) {
            MessageBox(NULL, _T("An error occured while exporting the current file!"), _T("Error"), MB_OK | MB_ICONERROR);
        }
        return;
    }

    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH]{};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("Desktop Files\0*.desktop\0All Files\0*.*\0");  // Filter for INI files
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = _T("Save");
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE && !g_icm->Export(ofn.lpstrFile))
    {
        MessageBox(NULL, _T("An error occured while exporting the current file!"), _T("Error"), MB_OK | MB_ICONERROR);
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
    switch (message)
    {
    case WM_INITDIALOG:
    {   
        auto hr = CoInitialize(NULL);
        hr |= CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pProgressDialog));
        if (FAILED(hr))
        {
            CoUninitialize();
        }

        //IShellWindows* pShellWindows;
        //hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&pShellWindows));

        //long lhwnd;
        //VARIANT loc{ CSIDL_DESKTOP };
        //VariantInit(&loc);
        //VARIANT empty{};
        //VariantInit(&empty);
        //IDispatch* pDisp;
        //hr = pShellWindows->FindWindowSW(&loc, &empty, SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &pDisp);

        //IServiceProvider* pService;
        //pDisp->QueryInterface(IID_PPV_ARGS(&pService));

        //IShellBrowser* pBrowser;
        //pService->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&pBrowser));

        //IShellView* pView;
        //pBrowser->QueryActiveShellView(&pView);

        //IFolderView* pFolderView;
        //pView->QueryInterface(IID_PPV_ARGS(&pFolderView));

        //IShellFolder* pFolder;
        //pFolderView->GetFolder(IID_PPV_ARGS(&pFolder));

        //IEnumIDList* pEnum;
        //pFolderView->Items(SVGIO_ALLVIEW, IID_PPV_ARGS(&pEnum));

        //for (ITEMID_CHILD* pIdl; pEnum->Next(1, &pIdl, nullptr) == S_OK; CoTaskMemFree(pIdl)) {
        //    STRRET str;
        //    pFolder->GetDisplayNameOf(pIdl, SHGDN_NORMAL, &str);

        //    wchar_t* spszName;
        //    StrRetToStr(&str, pIdl, &spszName);

        //    OutputDebugString(spszName);
        //    OutputDebugString(_T("\n"));

        //    POINT pt;
        //    pFolderView->GetItemPosition(pIdl, &pt);
        //}

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
            LoadFile();
            break;
        case IDM_SAVE:
            SaveFile();
            break;
        case IDC_RESTORE:
        case IDC_SAVE:
        {
            HWND comboBox = GetDlgItem(hWnd, IDC_COMBO);
            
            TCHAR text[MAX_PATH];
            ComboBox_GetText(comboBox, text, MAX_PATH);

            auto absPath = _tfullpath(NULL, text, MAX_PATH);

            if (wmId == IDC_RESTORE) 
            {
                auto result = MessageBox(hWnd, _T("This will apply the selected profile onto your desktop."), _T("Confirm"), MB_OKCANCEL | MB_ICONWARNING);
                if (result == IDOK) {
                    LoadFile(absPath);
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
