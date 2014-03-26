#include "player/application.h"

#include <QIcon>

#include <librarymanager.h>
#include "mainwindow.h"

#ifdef Q_OS_MAC
 #include <objc/objc.h>
 #include <objc/message.h>

bool dockClickHandler(id self, SEL _cmd, ...)
{
    Q_UNUSED(self);
    Q_UNUSED(_cmd);
    ( (Application *) qApp )->onClickOnDock();
    return true;
}
#endif

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv)
{
    setApplicationDisplayName("Simple Player");
    setApplicationName("SimplePlayer");
    setApplicationVersion("1.0.0");
    setOrganizationDomain("br.cefetmg");
    setOrganizationName("CEFET-MG");

    setWindowIcon( QIcon(":/icon.png") );
    setQuitOnLastWindowClosed(false);

#ifdef Q_OS_MAC
    Class       cls               = (Class) objc_getClass("NSApplication");
    SEL         sharedApplication = sel_registerName("sharedApplication");
    objc_object *appInst          = objc_msgSend( (objc_object *) cls, sharedApplication );

    if ( appInst != NULL ) {
        objc_object *delegate = objc_msgSend( appInst, sel_registerName("delegate") );
        objc_object *delClass = objc_msgSend( delegate, sel_registerName("class") );

        class_addMethod( (Class) delClass, sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:"), (IMP) dockClickHandler, "B@:" );
    }
#endif
}

void Application::aboutToQuit()
{
    LibraryManager::instance()->stopScan();
}

void Application::onClickOnDock()
{
    MainWindow::instance()->show();
}
