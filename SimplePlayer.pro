#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T19:26:47
#
#-------------------------------------------------

# Requires Qt5
# Tested with Qt 5.2.0

macx:QMAKE_CXX = "ccache clang -Qunused-arguments -fcolor-diagnostics"
linux-g++:QMAKE_CXX = "ccache g++"

QT       += core gui multimedia sql widgets multimediawidgets

CONFIG += c++11

TARGET = SimplePlayer
TEMPLATE = app

SOURCES += player/main.cpp\
           player/mainwindow.cpp \
           player/settings.cpp \
           player/playlistmodel.cpp \
           player/videowidget.cpp \
    player/application.cpp \
    player/musictreeitemmodel.cpp

HEADERS  += player/mainwindow.h \
            player/settings.h \
            player/playlistmodel.h \
            player/videowidget.h \
    player/application.h \
    player/musictreeitemmodel.h

FORMS    += ui/mainwindow.ui \
            ui/settings.ui

include(taglib/taglib.pri)
include(libmedia/libmedia.pri)
