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
	jsonindex.cpp \
    json.cpp \
    postdata.cpp \
    balancedialog.cpp \
    jconfig.cpp \
    paymentdialog.cpp \
    ejectdialog.cpp \
	printer.cpp

HEADERS  += mainwindow.h \
    pindialog.h \
    cardreader.h \
    bytearray.h \
    command.h \
    debug.h \
    terminal.h \
	jsonindex.h \
    json.h \
    postdata.h \
    balancedialog.h \
    jconfig.h \
    paymentdialog.h \
    ejectdialog.h \
	printer.h \
	config.h

FORMS    += mainwindow.ui \
    pindialog.ui \
    balancedialog.ui \
    paymentdialog.ui \
    ejectdialog.ui

OTHER_FILES +=
