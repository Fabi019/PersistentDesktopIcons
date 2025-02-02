#include "IconManager.h"

IconManager::IconManager() {
    HWND desktop = NULL;
    EnumWindows(IconManager::FindShellDefView, (LPARAM)&desktop);

    listView = FindWindowEx(desktop, NULL, _T("SysListView32"), NULL);

    DWORD pid = NULL;
    GetWindowThreadProcessId(listView, &pid);

    explorer = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, pid);

    // Memory allocated within the explorer process
    remoteData = VirtualAllocEx(explorer, NULL, max(sizeof(LVITEM), sizeof(POINT)), MEM_COMMIT, PAGE_READWRITE);
    remoteText = static_cast<LPTSTR>(VirtualAllocEx(explorer, NULL, sizeof(TCHAR) * (MAX_PATH + 1), MEM_COMMIT, PAGE_READWRITE));
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

    if (SendMessage(listView, LVM_GETITEMTEXT, index, (LPARAM)remoteData) <= 0) {
        return false;
    }

    ReadProcessMemory(explorer, remoteText, &info.text, sizeof(info.text), NULL);

    return true;
}

bool IconManager::Export(const LPTSTR filePath) {
    IconManager::IconInfo info{};
    TCHAR buffer[_MAX_U64TOSTR_BASE10_COUNT]{};

    BOOL success = true;

    for (int i = 0; i < GetIconCount(); ++i)
    {
        if (!GetIconInfo(i, info)) 
        {
            success = false;
            continue;
        }

        _ltot_s(info.position.x, buffer, 10);
        success |= WritePrivateProfileString(info.text, _T("X"), buffer, filePath);

        _ltot_s(info.position.y, buffer, 10);
        success |= WritePrivateProfileString(info.text, _T("Y"), buffer, filePath);
    }

    return success;
}

bool IconManager::Import(const LPTSTR filePath, IProgressDialog* progress, int offsetX, int offsetY, bool alignAfter) {
    const auto total = GetIconCount();
    const auto style = ListView_GetExtendedListViewStyle(listView);

    // Disable snap to grid while moving icons
    ListView_SetExtendedListViewStyleEx(listView, LVS_EX_SNAPTOGRID, 0);

    IconManager::IconInfo info{};
    for (int i = 0; i < total; ++i)
    {
        if (!GetIconInfo(i, info)) {
            continue;
        }

        if (progress)
        {
            if (progress->HasUserCancelled()) {
                break;
            }

            progress->SetProgress(i, total);

#ifndef _UNICODE
            wchar_t buffer[MAX_PATH];
            MultiByteToWideChar(CP_UTF8, 0, info.text, -1, buffer, MAX_PATH);
            progress->SetLine(2, buffer, false, NULL);
#else
            progress->SetLine(2, info.text, false, NULL);
#endif // !UNICODE
        }

        auto x = GetPrivateProfileInt(info.text, _T("X"), 2147483647, filePath);
        auto y = GetPrivateProfileInt(info.text, _T("Y"), 2147483647, filePath);

        if (x == 2147483647 || y == 2147483647) {
            continue;
        }

        SetIconPosition(i, x + offsetX, y + offsetY);
    }

    // Restore original style
    ListView_SetExtendedListViewStyle(listView, style);
    if (alignAfter) {
        ListView_Arrange(listView, LVA_SNAPTOGRID);
    }

    return true;
}