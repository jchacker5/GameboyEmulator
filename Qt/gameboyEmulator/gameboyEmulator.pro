TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
QT += core widgets
SOURCES += \
        gameboy.cpp \
        main.cpp \
        screen.cpp \
        z80.cpp
        testrom.gb


HEADERS += \
    gameboy.h \
    screen.h \
    z80.h
