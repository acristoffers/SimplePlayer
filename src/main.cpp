#include <QApplication>

#include "database.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationDisplayName("Simple Player");
    a.setApplicationName("SimplePlayer");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationDomain("br.cefetmg");
    a.setOrganizationName("CEFET-MG");

    MainWindow w;
    w.show();

    Settings settings;
    while ( !settings.hasPath() ) {
        settings.show();
        settings.requestToAddPath();
    }

    DataBase::instance();

    LibraryManager::instance()->scan();

    return a.exec();
}
