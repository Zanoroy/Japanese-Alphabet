@echo off
REM Package Script for Japanese Alphabet Quiz
REM Run this from the japanese-alphabet-quiz folder

echo Creating release package...

REM Create clean release directory
if exist release-package rmdir /s /q release-package
mkdir release-package
mkdir release-package\profiles

REM Copy executable (adjust path if using Release build)
copy build\Desktop_Qt_6_9_2_MinGW_64_bit-Debug\japanese-alphabet-quiz.exe release-package\

REM Deploy Qt dependencies
c:\qt\6.9.2\mingw_64\bin\windeployqt.exe release-package\japanese-alphabet-quiz.exe

REM Copy vocabulary data
copy sample_vocabularies.json release-package\profiles\vocabularies.json

REM Copy icon
copy appicon.ico release-package\

REM Create README
echo Japanese Alphabet Quiz > release-package\README.txt
echo. >> release-package\README.txt
echo To run: Double-click japanese-alphabet-quiz.exe >> release-package\README.txt
echo. >> release-package\README.txt
echo The application will create a 'profiles' folder for saving your progress. >> release-package\README.txt

echo.
echo Package created in: release-package\
echo.
echo Zip this folder and share it!
pause
