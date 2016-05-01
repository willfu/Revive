#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T16:57:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += quick

TARGET = ReviveOverlay
TEMPLATE = app


SOURCES += main.cpp\
    openvroverlaycontroller.cpp \
    reviveinjectcontroller.cpp

HEADERS  += \
    openvroverlaycontroller.h \
    reviveinjectcontroller.h

DISTFILES += \
    OverlayForm.ui.qml \
    Overlay.qml \
    Oculus.js

INCLUDEPATH += ../openvr/headers

LIBS += -L../openvr/lib/win64 -lopenvr_api

Debug:DESTDIR = ../Debug
Release:DESTDIR = ../Release

RESOURCES += \
    overlay.qrc
