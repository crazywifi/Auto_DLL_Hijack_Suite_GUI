// minimal_debug_payload.cpp
#include <windows.h>
#include <stdio.h> // For wvsprintfW or _vsnwprintf_s

// Helper to output to DebugView
VOID OutputDebugStringFormat( LPCWSTR format, ... ) {
   WCHAR buf[1024];
   va_list args;
   va_start( args, format );
   #ifdef _MSC_VER
     _vsnwprintf_s(buf, sizeof(buf)/sizeof(WCHAR), _TRUNCATE, format, args);
   #else // For MinGW
     wvsprintfW( buf, format, args );
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

    // Log every call to DllMain, regardless of reason
    OutputDebugStringFormat(L"[minimal_debug_payload.dll] DllMain called. Reason: %lu (0x%X)\n", ul_reason_for_call, ul_reason_for_call);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        OutputDebugStringW(L"[minimal_debug_payload.dll] --> DLL_PROCESS_ATTACH entered.\n");
        
        // DO ABSOLUTELY NOTHING ELSE for this test, just try to return TRUE
        
        OutputDebugStringW(L"[minimal_debug_payload.dll] <-- DLL_PROCESS_ATTACH attempting to return TRUE.\n");
        return TRUE; 
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        OutputDebugStringW(L"[minimal_debug_payload.dll] --> DLL_PROCESS_DETACH entered.\n");
        OutputDebugStringW(L"[minimal_debug_payload.dll] <-- DLL_PROCESS_DETACH attempting to return TRUE.\n");
        return TRUE; 
    }
    else if (ul_reason_for_call == DLL_THREAD_ATTACH) {
        OutputDebugStringW(L"[minimal_debug_payload.dll] --> DLL_THREAD_ATTACH entered.\n");
        OutputDebugStringW(L"[minimal_debug_payload.dll] <-- DLL_THREAD_ATTACH attempting to return TRUE.\n");
        return TRUE;
    }
    else if (ul_reason_for_call == DLL_THREAD_DETACH) {
        OutputDebugStringW(L"[minimal_debug_payload.dll] --> DLL_THREAD_DETACH entered.\n");
        OutputDebugStringW(L"[minimal_debug_payload.dll] <-- DLL_THREAD_DETACH attempting to return TRUE.\n");
        return TRUE;
    }

    OutputDebugStringW(L"[minimal_debug_payload.dll] DllMain: Reached end, returning default TRUE for unhandled reason.\n");
    return TRUE; 
}