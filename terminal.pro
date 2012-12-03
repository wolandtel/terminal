#-------------------------------------------------
#
# Project created by QtCreator 2012-09-13T15:11:21
#
#-------------------------------------------------

DEFINES += DEBUG

QT += core gui network script

CONFIG += console

unix:INCLUDEPATH += /usr/include/qcommandline
win32:INCLUDEPATH += "C:/Program Files/qcommandline/include/qcommandline"
win32:LIBS += -L "c:/Qt/4.8.3/lib" -L "C:/Program Files/qcommandline/lib"

LIBS += -lqcommandline

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
	printer.cpp \
	ipc.cpp \
	options.cpp \
	logger.cpp \
	logtarget.cpp

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
	config.h \
	ipc.h \
	options.h \
	logger.h \
	logtarget.h

FORMS    += mainwindow.ui \
    pindialog.ui \
    balancedialog.ui \
    paymentdialog.ui \
    ejectdialog.ui

OTHER_FILES +=
