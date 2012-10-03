#-------------------------------------------------
#
# Project created by QtCreator 2012-09-13T15:11:21
#
#-------------------------------------------------

QT       += core gui network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(3rdparty/qextserialport-1.2beta2/src/qextserialport.pri)

TARGET = terminal
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pindialog.cpp \
    cardreader.cpp \
    bytearray.cpp \
    command.cpp \
    terminal.cpp \
    json.cpp \
    postdata.cpp

HEADERS  += mainwindow.h \
    pindialog.h \
    cardreader.h \
    bytearray.h \
    command.h \
    debug.h \
    terminal.h \
    json.h \
    postdata.h

FORMS    += mainwindow.ui \
    pindialog.ui

OTHER_FILES +=
