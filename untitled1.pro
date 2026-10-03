QT += core gui widgets

CONFIG += c++11

TARGET = Simple_Pass_0.1.0
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

# --- ПРИНУДИТЕЛЬНАЯ ПОДДЕРЖКА WINDOWS XP ---
win32 {
    # Указываем подсистему Windows 5.01 (это Windows XP)
    QMAKE_LFLAGS += -Wl,--subsystem,windows:5.01
    QMAKE_CFLAGS += -DWINVER=0x0501 -D_WIN32_WINNT=0x0501
    QMAKE_CXXFLAGS += -DWINVER=0x0501 -D_WIN32_WINNT=0x0501
}

# --- ПУТИ К OPENSSL 1.0.2 (Win32) ---
# Замените путь, если вы установили OpenSSL 1.0.2 в другое место
INCLUDEPATH += "C:/Program Files (x86)/OpenSSL-Win32/include"
LIBS += "C:/Program Files (x86)/OpenSSL-Win32/shared/lib/libeay32.lib"
RC_ICONS += simplepass.ico
