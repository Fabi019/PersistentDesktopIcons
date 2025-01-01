#pragma once
#include "framework.h"

class IconManager {
public:
    struct IconInfo {
        TCHAR text[MAX_PATH + 1];
        POINT position;
    };

    // Constructor
    IconManager();

    // Destructor
    ~IconManager()
    {
        if (remoteData) {
            VirtualFreeEx(explorer, remoteData, 0, MEM_RELEASE);
            remoteData = nullptr;
        }

        if (remoteText) {
            VirtualFreeEx(explorer, remoteText, 0, MEM_RELEASE);
            remoteText = nullptr;
        }

        if (explorer) {
            CloseHandle(explorer);
            explorer = nullptr;
        }
    }

    int GetIconCount();

    bool GetIconInfo(int index, IconInfo& info);

    bool SetIconPosition(int index, long x, long y);

    bool Export(const TCHAR* filePath);

    bool Import(const TCHAR* filePath, IProgressDialog* progress = nullptr);

    static BOOL CALLBACK FindShellDefView(HWND child, LPARAM lparam)
    {
        TCHAR name[MAX_PATH];
        GetClassName(child, name, MAX_PATH);

        if (_tcscmp(name, _T("WorkerW")) == 0 || _tcscmp(name, _T("Progman")) == 0) {
            HWND defView = FindWindowEx(child, NULL, _T("SHELLDLL_DefView"), NULL);
            
            if (defView != NULL) {
                *reinterpret_cast<HWND*>(lparam) = defView;
                return FALSE;   // stop
            }
        }

        return TRUE;    // continue
    }

private:
    HANDLE explorer;
    HWND listView;
    LPVOID remoteData;
    LPVOID remoteText;
};
