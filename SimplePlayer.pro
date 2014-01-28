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

CONFIG(release) {
    QMAKE_CFLAGS_RELEASE -= -O
    QMAKE_CFLAGS_RELEASE -= -O1
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CFLAGS_RELEASE += -O3

    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
}

TARGET = SimplePlayer
TEMPLATE = app

include(taglib/taglib.pri)
include(libmedia/libmedia.pri)

SOURCES += player/main.cpp\
           player/mainwindow.cpp \
           player/settings.cpp \
           player/playlistmodel.cpp \
           player/videowidget.cpp \
           player/application.cpp \
           player/musictreeitemmodel.cpp \
           player/playlistlistview.cpp \
           player/playercontrols.cpp \
           player/videomodel.cpp \
           player/imagemodel.cpp

HEADERS  += player/mainwindow.h \
            player/settings.h \
            player/playlistmodel.h \
            player/videowidget.h \
            player/application.h \
            player/musictreeitemmodel.h \
            player/playlistlistview.h \
            player/playercontrols.h \
            player/videomodel.h \
            player/imagemodel.h

FORMS    += ui/mainwindow.ui \
            ui/settings.ui \
            ui/playercontrols.ui

RESOURCES += res/Resources.qrc

RC_ICONS = res/icon.ico

ICON = res/icon.icns
