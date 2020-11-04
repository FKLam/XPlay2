#-------------------------------------------------
#
# Project created by QtCreator 2020-10-31T14:18:21
#
#-------------------------------------------------

QT       += core gui multimedia
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XPlay2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        XPlay2.cpp \
    XDemux.cpp \
    XDecode.cpp \
    XPlayVideoWidget.cpp \
    XResample.cpp \
    XAudioPlay.cpp \
    XAudioThread.cpp \
    XVideoThread.cpp \
    XDemuxThread.cpp \
    XDecodeThread.cpp \
    XSlider.cpp

HEADERS += \
        XPlay2.h \
    XDemux.h \
    XDecode.h \
    XPlayVideoWidget.h \
    XResample.h \
    XAudioPlay.h \
    XAudioThread.h \
    XVideoThread.h \
    IVideoCall.h \
    XDemuxThread.h \
    XDecodeThread.h \
    XSlider.h

FORMS += \
        XPlay2.ui

INCLUDEPATH += $$PWD/../../include/ -framework CoreFoundation
LIBS += -L$$PWD/../../libs/
LIBS += -lavformat -lavcodec -lavutil -lswscale -lswresample
