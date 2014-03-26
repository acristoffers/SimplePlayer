#include <QTimer>

#include <librarymanager.h>

#include "application.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);

    MainWindow::instance()->show();

    QTimer::singleShot( 2000, LibraryManager::instance(), SLOT( startScan() ) );

    return a.exec();
}
