// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Comctl32.lib")

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <commctrl.h>
#include <commdlg.h>
#include <shlobj_core.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windowsx.h>
#include <shellapi.h>