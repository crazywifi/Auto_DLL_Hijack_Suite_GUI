#!/bin/bash

# Ensure MinGW-w64 is installed: sudo apt install mingw-w64

# --- CONFIGURATION ---
# Target architecture:
# For 64-bit Windows: TARGET_ARCH_PREFIX="x86_64-w64-mingw32-"
# For 32-bit Windows: TARGET_ARCH_PREFIX="i686-w64-mingw32-"
TARGET_ARCH_PREFIX="i686-w64-mingw32-" # << SET THIS TO MATCH YOUR VulnerableApp_advance.exe

# Source files and output DLL names
PAYLOADS=(
    "notepad_payload.cpp:notepad.dll"
    "calcsigned_payload.cpp:calcsigned.dll"
    "opencmd_payload.cpp:opencmd_dll.dll"
	"FileCreation.cpp:FileCreation.dll"
	"curl_payload.cpp:curl_payload.dll"
    # Add any other payloads you have, like powershell_payload.cpp:powershell.dll
)

# Common compiler flags
# -Wall -Wextra: Enable more warnings
# -O2: Optimization level (optional, can use -Og for debug)
# -shared: Create a shared library (DLL)
# -static-libgcc -static-libstdc++: Link standard libraries statically for better portability
# -DUNICODE -D_UNICODE: Ensure wide character support is consistently used (good for Windows API)
COMMON_FLAGS="-Wall -Wextra -O2 -shared -static-libgcc -static-libstdc++ -DUNICODE -D_UNICODE"

# Libraries to link (user32 for MessageBox if used, kernel32 for most things)
# Not strictly needed for CreateProcessW in this simple DllMain, but good practice.
# For wvsprintfW, linking -luser32 might be needed by some MinGW versions if <stdio.h> doesn't pull it.
LINK_LIBS="-lkernel32 -luser32"


# --- COMPILE LOOP ---
echo "Compiling payloads for Windows (${TARGET_ARCH_PREFIX}g++)..."
mkdir -p compiled_payloads # Create a directory for output DLLs

for item in "${PAYLOADS[@]}"; do
    SRC_FILE="${item%%:*}"  # Get source file part (before :)
    DLL_NAME="${item##*:}"  # Get DLL name part (after :)
    OUTPUT_PATH="compiled_payloads/${DLL_NAME}"

    if [ ! -f "$SRC_FILE" ]; then
        echo "Source file '$SRC_FILE' not found. Skipping."
        continue
    fi

    echo "  Compiling '$SRC_FILE' -> '$OUTPUT_PATH'..."
    COMMAND="${TARGET_ARCH_PREFIX}g++ ${COMMON_FLAGS} \"$SRC_FILE\" -o \"$OUTPUT_PATH\" ${LINK_LIBS}"
    echo "    Executing: $COMMAND"
    
    eval $COMMAND # Using eval to handle quotes in paths correctly if any spaces

    if [ $? -eq 0 ]; then
        echo "    SUCCESS: '$OUTPUT_PATH' compiled."
    else
        echo "    ERROR: Failed to compile '$SRC_FILE'."
    fi
    echo ""
done

echo "Compilation finished. Check the 'compiled_payloads' directory."