# Simple Pass
A secure, 100% offline password manager built with C++ and Qt 5.6.3, featuring AES-256-CBC encryption via OpenSSL. Designed with legacy compatibility to run seamlessly on Windows XP and newer systems, offering complete local credential management with zero cloud telemetry. This project is experimental and created with/by AI.
# Installation
To use Simple Pass, you need to unzip zip-file and run the program.
# Necessary files
All necessary files are in zip. Application folder and file structure must be like that:
(Folder with Simple Pass)/
│
├── Simple_Pass_0.1.0.exe      <-- Program
├── libeay32.dll               <-- OpenSSL
├── Qt5Core.dll                <-- Qt Core
├── Qt5Gui.dll                 <-- Qt GUI
├── Qt5Widgets.dll             <-- Qt Widgets
├── libstdc++-6.dll            <-- MinGW C++ runtime
├── libgcc_s_dw2-1.dll         <-- MinGW GCC runtime
├── libwinpthread-1.dll        <-- MinGW winpthread
│
└── platforms/                 <-- Folder with plugins
    └── qwindows.dll           <-- Needed interface plugin
# Used programs
In this project i used:
# 1.Qt 5.6.3 - for creating project;
# 2.OpenSSL 1.0.2 - for encryption.
