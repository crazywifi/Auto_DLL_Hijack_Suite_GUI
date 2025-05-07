#!/usr/bin/env bash

# Define the MinGW-w64 compiler prefix for 64-bit Windows
# If you installed a different variant or want 32-bit, adjust this.
# For 32-bit: i686-w64-mingw32-
COMPILER_PREFIX="x86_64-w64-mingw32-"

# Source files
APP_SRC="VulnerableApp.cpp"
LIB_SRC="HijackableLib.cpp"

# Output names
APP_EXE="VulnerableApp.exe"
PAYLOAD_DLL="NonExistentLib.dll"

echo "Compiling ${APP_SRC} into ${APP_EXE} (64-bit Windows)..."
${COMPILER_PREFIX}g++ ${APP_SRC} -o ${APP_EXE} \
    -static-libgcc -static-libstdc++ \
    -Wall -Wextra \
    -luser32 -lkernel32 # Explicitly link common Windows libraries

if [ $? -ne 0 ]; then
    echo "Failed to compile ${APP_EXE}"
    exit 1
fi
echo "${APP_EXE} compiled successfully."
echo ""

echo "Compiling ${LIB_SRC} into ${PAYLOAD_DLL} (64-bit Windows)..."
${COMPILER_PREFIX}g++ ${LIB_SRC} -shared -o ${PAYLOAD_DLL} \
    -static-libgcc -static-libstdc++ \
    -Wall -Wextra \
    -Wl,--out-implib,lib${PAYLOAD_DLL%.*}.a # Optional: create import library

if [ $? -ne 0 ]; then
    echo "Failed to compile ${PAYLOAD_DLL}"
    exit 1
fi
echo "${PAYLOAD_DLL} compiled successfully."
echo ""

echo "--- CROSS-COMPILE COMPLETE (for 64-bit Windows) ---"
echo "You should find:"
echo "  - ${APP_EXE}"
echo "  - ${PAYLOAD_DLL}"
echo ""
echo "Instructions:"
echo "1. Transfer these files to a Windows machine for testing."
echo "2. Place ${APP_EXE} in a directory like C:\\TestApp\\"
echo "3. Place ${PAYLOAD_DLL} into your Python script's 'Payloads' directory on the Windows machine."