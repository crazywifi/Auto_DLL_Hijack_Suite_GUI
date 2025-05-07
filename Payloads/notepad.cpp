// notepad_payload.cpp
#include <windows.h>
#include <stdio.h> // For wvsprintfW if not implicitly included by windows.h with mingw

// Helper to output to DebugView
VOID OutputDebugStringFormat( LPCWSTR format, ... ) {
   WCHAR buf[1024];
   va_list args;
   va_start( args, format );
   // Use _vsnwprintf_s for safety if available and preferred,
   // but wvsprintfW is common with MinGW.
   // If wvsprintfW is not found, try _vsnwprintf or check MinGW includes.
   #ifdef _MSC_VER // For Visual Studio
     _vsnwprintf_s(buf, sizeof(buf)/sizeof(WCHAR), _TRUNCATE, format, args);
   #else // For MinGW
     wvsprintfW( buf, format, args ); // Standard WinAPI
   #endif
   va_end( args );
   OutputDebugStringW( buf );
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
    (void)hModule;
    (void)lpReserved;

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        OutputDebugStringW(L"[notepad_payload.dll] DLL_PROCESS_ATTACH entered.\n");
        
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        WCHAR cmd[] = L"notepad.exe"; // For notepad.dll

        if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            OutputDebugStringFormat(L"[notepad_payload.dll] CreateProcessW for notepad.exe SUCCEEDED. Process ID: %lu, Thread ID: %lu\n", pi.dwProcessId, pi.dwThreadId);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            DWORD dwError = GetLastError();
            OutputDebugStringFormat(L"[notepad_payload.dll] CreateProcessW for notepad.exe FAILED. Error: %d (0x%X)\n", dwError, dwError);
        }
        
        OutputDebugStringW(L"[notepad_payload.dll] Reached end of DLL_PROCESS_ATTACH, returning TRUE.\n");
        return TRUE; // CRITICAL
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        OutputDebugStringW(L"[notepad_payload.dll] DLL_PROCESS_DETACH.\n");
    }
    // else if (ul_reason_for_call == DLL_THREAD_ATTACH) {
    //     OutputDebugStringW(L"[notepad_payload.dll] DLL_THREAD_ATTACH.\n");
    // }
    // else if (ul_reason_for_call == DLL_THREAD_DETACH) {
    //     OutputDebugStringW(L"[notepad_payload.dll] DLL_THREAD_DETACH.\n");
    // }
    return TRUE; 
}