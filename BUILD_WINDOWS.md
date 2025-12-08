# Building on Windows

## Prerequisites

1. **Install Qt6 for Windows**
   - Download from: https://www.qt.io/download-qt-installer
   - During installation, select:
     - Qt 6.x for Windows (MinGW or MSVC)
     - Qt Creator (optional but recommended)
     - MinGW compiler (if not using MSVC)

2. **Install CMake**
   - Download from: https://cmake.org/download/
   - Or use Chocolatey: `choco install cmake`
   - Make sure to add CMake to PATH during installation

3. **Install Git** (if not already installed)
   - Download from: https://git-scm.com/download/win

## Build Instructions

### Method 1: Using Command Line (MinGW)

Open Command Prompt or PowerShell:

```cmd
cd japanese-alphabet-quiz
mkdir build
cd build

# Configure (adjust Qt6_DIR path if needed)
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\mingw_64"

# Build
mingw32-make

# Run
japanese-alphabet-quiz.exe
```

### Method 2: Using Command Line (Visual Studio)

Open Command Prompt or PowerShell:

```cmd
cd japanese-alphabet-quiz
mkdir build
cd build

# Configure (adjust Qt6_DIR path if needed)
cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2022_64"

# Build
cmake --build . --config Release

# Run
Release\japanese-alphabet-quiz.exe
```

### Method 3: Using Qt Creator (Easiest)

1. Open Qt Creator
2. File → Open File or Project
3. Select `CMakeLists.txt` from the `japanese-alphabet-quiz` folder
4. Select a kit (MinGW or MSVC)
5. Click "Configure Project"
6. Click the green "Run" button (or press Ctrl+R)

## Notes

- The executable will be in the `build` directory
- Make sure to copy `sample_vocabularies.json` to `build/profiles/vocabularies.json` or create a symlink
- The application icon (`appicon.ico`) is automatically embedded during build
- Qt DLLs need to be in the same directory as the .exe for distribution

## Troubleshooting

**If Qt is not found:**
- Set CMAKE_PREFIX_PATH to your Qt installation, e.g.:
  ```cmd
  cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\mingw_64"
  ```

**If you get "generator not found" errors:**
- For MinGW: Install MinGW from Qt installer
- For Visual Studio: Install Visual Studio with C++ tools

**Missing DLLs when running:**
- Use Qt's `windeployqt.exe` tool:
  ```cmd
  C:\Qt\6.x.x\mingw_64\bin\windeployqt.exe japanese-alphabet-quiz.exe
  ```
