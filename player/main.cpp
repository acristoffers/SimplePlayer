#include <QTimer>

#include <QDebug>

#include <librarymanager.h>
#include <music.h>

#include "application.h"
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);

    MainWindow w;

    w.show();

    Settings settings(&w);
    while ( !settings.hasPath() ) {
        settings.show();
        settings.requestToAddPath();
    }

    QTimer::singleShot( 2000, LibraryManager::instance(), SLOT( startScan() ) );

    return a.exec();
}
