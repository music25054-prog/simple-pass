# Simple Pass
A secure, 100% offline password manager built with C++ and Qt 5.6.3, featuring AES-256-CBC encryption via OpenSSL. Designed with legacy compatibility to run seamlessly on Windows XP and newer systems, offering complete local credential management with zero cloud telemetry. This project is experimental and created with/by AI.
# Installation
To use Simple Pass, you need to unzip zip-file and run the program.
# Necessary files
All necessary files are in zip. Application folder and file structure must be like that:
`Simple Pass/` ➔ `Simple_Pass_0.1.0.exe` |`simplepass.ico` | `libeay32.dll` | `Qt5Core.dll` | `Qt5Cored.dll` | `Qt5Gui.dll` |`Qt5Guid.dll` | `Qt5Widgets.dll` |`Qt5Widgetsd.dll` | `libstdc++-6.dll` | `libgcc_s_dw2-1.dll` | `libwinpthread-1.dll` | `platforms/` (➔ `qwindows.dll`)

# Used programs
In this project i used:
- 1.Qt 5.6.3 - for creating project;
- 2.OpenSSL 1.0.2 - for encryption.
# Compilation
## Preparation
1. Make sure, that you have Qt Creator with Developer Kit Qt 5.6.3, OpenSSL 1.0.2 and good mood)
2. Place source files into one folder.
3. Open 'untitled1.pro' and ensure that the paths to the OpenSSL header files and library match their locations on your PC.
## The compilation itself
1. Open Qt Creator and open project from Qt.
2. During the kit configuration stage , be sure to select the 32-bit MinGW compiler and click Configure Project.
3. Click "Build" ➔ "Clean All".
4. Click "Build" ➔ "Run qmake".
5. Press Ctrl+R to build project.
After compilating don't forget to place necessary .dll files and 'platforms' folder like in the original zip-file!
