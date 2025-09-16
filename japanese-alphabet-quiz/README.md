# Hiragana Quiz Qt

A cross-platform (Windows/Linux) C++ Qt GUI application for practicing Hiragana, Katakana, and basic Kanji, inspired by the Python `hiragana_quiz.py` app.

## Features
- Practice Hiragana, Katakana, and Kanji to Romaji mapping
- Per-script and per-row selection
- Adjustable "times to show" for each character
- Score tracking (correct/retries)
- Preferences saved between runs
- Responsive table UI

## Build Instructions

### Prerequisites
- Qt 6 (or Qt 5) development libraries (install via your package manager or from qt.io)
- CMake 3.15+
- C++17 compiler (g++/clang/MSVC)

### Linux
```bash
cd japanese-alphabet-quiz
mkdir build && cd build
cmake ..
make
./japanese-alphabet-quiz
```

### Windows (MSVC or MinGW)
- Open a Qt command prompt
- Run:
```bat
cd japanese-alphabet-quiz
mkdir build
cd build
cmake .. -G "NMake Makefiles" # or "MinGW Makefiles"
nmake # or mingw32-make
japanese-alphabet-quiz.exe
```

## Notes
- Preferences are saved in a JSON file in the same directory as the executable.
- All logic and UI are implemented in C++/Qt for best cross-platform compatibility.
