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

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
        src/media.cpp \
        src/music.cpp \
        src/image.cpp \
        src/video.cpp \
        src/database.cpp \
        src/settings.cpp \
        src/librarymanager.cpp \
        src/librarymanagerprivate.cpp

HEADERS  += src/mainwindow.h \
            src/media.h \
            src/music.h \
            src/image.h \
            src/video.h \
            src/database.h \
            src/settings.h \
            src/librarymanager.h \
            src/librarymanagerprivate.h

FORMS    += ui/mainwindow.ui \
            ui/settings.ui

include(taglib/taglib.pri)
