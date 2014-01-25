#include "player/application.h"

#include <librarymanager.h>

Application::Application(int argc, char **argv) :
    QApplication(argc, argv)
{
    setApplicationDisplayName("Simple Player");
    setApplicationName("SimplePlayer");
    setApplicationVersion("1.0.0");
    setOrganizationDomain("br.cefetmg");
    setOrganizationName("CEFET-MG");
}

void Application::aboutToQuit()
{
    LibraryManager::instance()->stopScan();
}
