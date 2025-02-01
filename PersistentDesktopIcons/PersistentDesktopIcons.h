#pragma once

#include "resource.h"

INT_PTR CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY _tWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int);

int __stdcall wWinMainCRTStartup()
{
#ifdef _WIN32
    typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

    LPFN_ISWOW64PROCESS isWow64;
    BOOL bIsWow64 = FALSE;

    isWow64 = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
    if (isWow64 != NULL && isWow64(GetCurrentProcess(), &bIsWow64) && bIsWow64)
    {
        MessageBox(NULL, _T("This application only works when executed in the native architecture! Please use the 64bit version."), _T("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }
#endif // _WIN32

    return _tWinMain(GetModuleHandle(NULL), 0, PathGetArgs(GetCommandLine()), 0);
}

static void LoadFile(TCHAR* fileName, int offsetX = 0, int offsetY = 0, bool alignAfter = false);
static void SaveFile(TCHAR* fileName);

static int ParseCommandLine(const TCHAR* lpCmdLine, TCHAR arguments[][MAX_PATH], int max_args) {
    int argIndex = 0;
    int charIndex = 0;

    const TCHAR* s = lpCmdLine;
    while (*s) {
        while (*s == ' ' || *s == '\t')
            ++s;
        if (*s == '\0') break;

        // Handle quoted argument
        if (*s == '"') {
            ++s;
            while (*s && *s != '"') {
                if (*s == '\\' && *(s + 1) == '"')
                    ++s; // skip the backslash
                arguments[argIndex][charIndex++] = *s++;
                if (charIndex >= MAX_PATH - 1)
                    break;
            }
            if (*s == '"')
                ++s;
        }
        // Handle unquoted argument
        else {
            while (*s && *s != ' ' && *s != '\t') {
                arguments[argIndex][charIndex++] = *s++;
                if (charIndex >= MAX_PATH - 1)
                    break;
            }
        }

        arguments[argIndex][charIndex] = '\0';
        ++argIndex;
        charIndex = 0;

        if (argIndex >= max_args)
            break;
    }

    return argIndex;
}