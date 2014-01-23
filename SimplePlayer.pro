#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T19:26:47
#
#-------------------------------------------------

# Requires Qt5
# Tested with Qt 5.2.0

QT       += core gui multimedia sql widgets multimediawidgets

CONFIG += c++11

TARGET = SimplePlayer
TEMPLATE = app

SOURCES += player/main.cpp\
           player/mainwindow.cpp \
           player/settings.cpp

HEADERS  += player/mainwindow.h \
            player/settings.h

FORMS    += ui/mainwindow.ui \
            ui/settings.ui

include(taglib/taglib.pri)
include(libmedia/libmedia.pri)
