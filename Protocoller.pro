#-------------------------------------------------
#
# Project created by QtCreator 2018-04-25T09:39:09
#
#-------------------------------------------------

QT += core gui
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Protocoller
TEMPLATE = app

QMAKE_CXXFLAGS += /std:c++latest

DEFINES += \
    QT_DEPRECATED_WARNINGS \
    QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = $$_PRO_FILE_PWD_/bin

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    hw/interface.cpp \
    hw/tester.cpp \
    dataview.cpp \
    field.cpp \
    commandmodel.cpp \
    parcelmodel.cpp
#    myspinbox.cpp


HEADERS += \
    magicgetruntime.h \
    mainwindow.h \
    hw/interface.h \
    hw/myprotokol.h \
    hw/tester.h \
    dataview.h \
    field.h \
    commandmodel.h \
    parcelmodel.h
#    myspinbox.h


FORMS += \
    mainwindow.ui

DISTFILES += \


