#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T19:26:47
#
#-------------------------------------------------

# Requires Qt5

QT += core gui multimedia sql widgets multimediawidgets

CONFIG += c++11

CONFIG(release, debug|release) {
    macx {
        QMAKE_CXX       = "ccache clang++"
        QMAKE_CXXFLAGS += "-Qunused-arguments -fcolor-diagnostics"
        QMAKE_CC        = "ccache clang"
        QMAKE_CFLAGS   += "-Qunused-arguments -fcolor-diagnostics"
    }

    linux-g++ {
        QMAKE_CXX = "ccache g++"
        QMAKE_CC  = "ccache gcc"
    }

    QMAKE_CFLAGS_RELEASE -= -O
    QMAKE_CFLAGS_RELEASE -= -O1
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CFLAGS_RELEASE += -O3

    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
}

include(taglib/taglib.pri)
include(libmedia/libmedia.pri)

TARGET = SimplePlayer
TEMPLATE = app

SOURCES += player/main.cpp\
           player/mainwindow.cpp \
           player/settings.cpp \
           player/playlistmodel.cpp \
           player/application.cpp \
           player/musictreeitemmodel.cpp \
           player/playlistlistview.cpp \
           player/playercontrols.cpp \
           player/about.cpp

HEADERS  += player/mainwindow.h \
            player/settings.h \
            player/playlistmodel.h \
            player/application.h \
            player/musictreeitemmodel.h \
            player/playlistlistview.h \
            player/playercontrols.h \
            player/about.h

FORMS    += ui/mainwindow.ui \
            ui/settings.ui \
            ui/playercontrols.ui \
            ui/about.ui

RESOURCES += res/Resources.qrc

RC_ICONS = res/icon.ico

ICON = res/icon.icns
