#include "IconManager.h"

IconManager::IconManager() {
    HWND desktop = NULL;
    EnumWindows(IconManager::FindShellDefView, reinterpret_cast<LPARAM>(&desktop));

    listView = FindWindowEx(desktop, NULL, L"SysListView32", NULL);

    DWORD pid = NULL;
    GetWindowThreadProcessId(listView, &pid);

    explorer = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, pid);

    // Memory allocated within the explorer process
    remoteData = VirtualAllocEx(explorer, NULL, max(sizeof(LVITEM), sizeof(POINT)), MEM_COMMIT, PAGE_READWRITE);
    remoteText = static_cast<TCHAR*>(VirtualAllocEx(explorer, NULL, sizeof(TCHAR) * (MAX_PATH + 1), MEM_COMMIT, PAGE_READWRITE));
}

int IconManager::GetIconCount() {
    return ListView_GetItemCount(listView);
}

bool IconManager::SetIconPosition(int index, long x, long y) {
    return ListView_SetItemPosition(listView, index, x, y);
}

bool IconManager::GetIconInfo(int index, IconManager::IconInfo& info) {
    if (ListView_GetItemPosition(listView, index, remoteData) != TRUE) {
        return false;
    }

    ReadProcessMemory(explorer, remoteData, &info.position, sizeof(POINT), NULL);

    LVITEM item{};
    item.iSubItem = 0;
    item.cchTextMax = MAX_PATH;
    item.mask = LVIF_TEXT;
    item.pszText = (LPTSTR)remoteText;

    WriteProcessMemory(explorer, remoteData, &item, sizeof(LVITEM), NULL);
    if (SendMessage(listView, LVM_GETITEMTEXT, index, (LPARAM)remoteData) < 0) {
        return false;
    }

    ReadProcessMemory(explorer, remoteText, &info.text, sizeof(info.text), NULL);

    return true;
}

bool IconManager::Export(const TCHAR* filePath) {
    IconManager::IconInfo info{};
    TCHAR buffer[_MAX_U64TOSTR_BASE10_COUNT]{};

    for (int i = 0; i < GetIconCount(); ++i) {
        if (!GetIconInfo(i, info)) {
            continue;
        }

        _ltot_s(info.position.x, buffer, 10);
        WritePrivateProfileString(info.text, L"X", buffer, filePath);

        _ltot_s(info.position.y, buffer, 10);
        WritePrivateProfileString(info.text, L"Y", buffer, filePath);
    }

    return true;
}

bool IconManager::Import(const TCHAR* filePath) {
    const int bufferSize = 1024;
    TCHAR buffer[bufferSize]{};

    if (GetPrivateProfileSectionNames(buffer, bufferSize, filePath) <= 0) {
        return false;
    }

    TCHAR* section = buffer;
    while (*section) {
        auto x = GetPrivateProfileInt(section, _T("X"), MAXUINT, filePath);
        auto y = GetPrivateProfileInt(section, _T("Y"), MAXUINT, filePath);

        if (x == MAXUINT || y == MAXUINT) {
            continue;
        }

        IconManager::IconInfo info{};
        for (int i = 0; i < GetIconCount(); ++i) {
            if (!GetIconInfo(i, info) || info.text == NULL) {
                continue;
            }

            if (_tcscmp(info.text, section) == 0) {
                SetIconPosition(i, x, y);
            }
        }

        section += _tcsclen(section) + 1;
    }

    return true;
}