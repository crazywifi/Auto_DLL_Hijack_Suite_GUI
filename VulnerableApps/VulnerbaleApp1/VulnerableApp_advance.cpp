#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::seconds

// Define the names of the DLLs we will try to load
const std::vector<std::string> DLL_NAMES_TO_TRY = {
    "NonExistentLib1.dll", // This should be your primary target for the payload
    "AnotherMissingLib.dll",
    "YetAnotherTry.dll"
};

// Optional: A legitimate system DLL to see successful loads too
const char* LEGIT_DLL_NAME = "user32.dll";


typedef void (*SayHelloFunc)();

int main(int argc, char* argv[]) {
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "VulnerableApp (Refined Test Version)" << std::endl;
    std::cout << "PID: " << GetCurrentProcessId() << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "This app will attempt to load several DLLs." << std::endl;
    std::cout << "Check Procmon for 'NAME NOT FOUND' for:" << std::endl;
    for (const auto& dll_name : DLL_NAMES_TO_TRY) {
        std::cout << "  - " << dll_name << std::endl;
    }
    std::cout << std::endl;

    // Give Procmon a moment if it was just launched externally
    // This is usually not needed as Procmon hooks quickly, but for testing...
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // --- Attempt to load a legitimate DLL first (for baseline) ---
    std::cout << "[INFO] Attempting to load legitimate DLL: " << LEGIT_DLL_NAME << std::endl;
    HMODULE hLegitDll = LoadLibraryA(LEGIT_DLL_NAME);
    if (hLegitDll == NULL) {
        std::cerr << "  [ERROR] Failed to load " << LEGIT_DLL_NAME << ". Error code: " << GetLastError() << std::endl;
    } else {
        std::cout << "  [SUCCESS] Successfully loaded " << LEGIT_DLL_NAME << "!" << std::endl;
        FreeLibrary(hLegitDll);
    }
    std::cout << "------------------------------------------" << std::endl;


    // --- Loop and attempt to load the target non-existent DLLs ---
    // We can loop a few times to give more chances for Procmon to catch it
    // if there's any extreme startup race condition (unlikely but for robustness).
    for (int i = 0; i < 2; ++i) { // Try 2 times
        std::cout << "\n[ATTEMPT " << i + 1 << "]" << std::endl;
        for (const auto& dll_name_str : DLL_NAMES_TO_TRY) {
            const char* current_dll_name = dll_name_str.c_str();
            std::cout << "[INFO] Attempting to load: " << current_dll_name << std::endl;

            HMODULE hDll = LoadLibraryA(current_dll_name);

            if (hDll == NULL) {
                std::cerr << "  [EXPECTED_FAILURE] Failed to load " << current_dll_name
                          << ". Error code: " << GetLastError() << " (This is expected for testing DLL hijacking)" << std::endl;
                // Procmon should show "NAME NOT FOUND" for this DLL
                // in the application's directory and then other search paths.
            } else {
                // This block would execute if your Python script successfully places a payload
                std::cout << "  [UNEXPECTED_SUCCESS] Successfully loaded " << current_dll_name << "!" << std::endl;
                std::cout << "                     This means a DLL was found (e.g., payload placed by script)." << std::endl;

                SayHelloFunc pSayHello = (SayHelloFunc)GetProcAddress(hDll, "SayHello");
                if (pSayHello != NULL) {
                    std::cout << "  [PAYLOAD] Found SayHello function. Calling it..." << std::endl;
                    pSayHello();
                } else {
                     std::cerr << "  [PAYLOAD_WARN] Could not find SayHello function in " << current_dll_name << ". Error: " << GetLastError() << std::endl;
                }
                FreeLibrary(hDll);
            }
             // Add a small delay between attempts
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        if (i < 1) { // If not the last outer loop iteration
            std::cout << "\nWaiting a second before next round of attempts..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    std::cout << "------------------------------------------" << std::endl;
    std::cout << "VulnerableApp: Test attempts complete." << std::endl;

    // Keep console open if not run with /autoexit argument
    bool autoExit = false;
    if (argc > 1 && std::string(argv[1]) == "/autoexit") {
        autoExit = true;
        std::cout << "Auto-exiting in 3 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    if (!autoExit) {
        std::cout << "Press Enter to exit." << std::endl;
        std::cin.get();
    }
    return 0;
}