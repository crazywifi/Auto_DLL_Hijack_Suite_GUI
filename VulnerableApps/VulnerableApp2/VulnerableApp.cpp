#include <windows.h>
#include <iostream>

// Define the name of the DLL we will try to load
const char* DLL_NAME = "NonExistentLib.dll"; // This is the DLL your script will target

typedef void (*SayHelloFunc)(); // Define a function pointer type for a function in the DLL

int main() {
    std::cout << "VulnerableApp: Starting..." << std::endl;
    std::cout << "VulnerableApp: Attempting to load " << DLL_NAME << std::endl;

    // Attempt to load the DLL by its name only (vulnerable part)
    HMODULE hDll = LoadLibraryA(DLL_NAME);

    if (hDll == NULL) {
        std::cerr << "VulnerableApp: Failed to load " << DLL_NAME << ". Error code: " << GetLastError() << std::endl;
        // Procmon should show "NAME NOT FOUND" for NonExistentLib.dll
        // in the application's directory and then other search paths.
    } else {
        std::cout << "VulnerableApp: Successfully loaded " << DLL_NAME << "!" << std::endl;

        // Try to get a function from the loaded DLL
        SayHelloFunc pSayHello = (SayHelloFunc)GetProcAddress(hDll, "SayHello");
        if (pSayHello == NULL) {
            std::cerr << "VulnerableApp: Failed to get address of SayHello function. Error code: " << GetLastError() << std::endl;
        } else {
            std::cout << "VulnerableApp: Found SayHello function. Calling it..." << std::endl;
            pSayHello(); // Call the function from the DLL
        }

        FreeLibrary(hDll); // Unload the DLL
        std::cout << "VulnerableApp: Unloaded " << DLL_NAME << "." << std::endl;
    }

    std::cout << "VulnerableApp: Press Enter to exit." << std::endl;
    std::cin.get(); // Keep console open
    return 0;
}