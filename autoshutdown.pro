QT       += core gui widgets

TARGET = AutoShutdown
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

CONFIG += c++17

SOURCES += \
        main.cpp \
        autoshutdown.cpp \
        power.cpp \
        registry.cpp \
        warning.cpp

HEADERS += \
        autoshutdown.h \
        power.h \
        registry.h \
        warning.h

FORMS += \
        autoshutdown.ui \
        warning.ui

RC_ICONS = autoshutdown.ico

RESOURCES += \
    autoshutdown.qrc
