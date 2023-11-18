#include "player/application.h"

#include <QIcon>

#include <librarymanager.h>
#include "mainwindow.h"

#ifdef Q_OS_MAC
 #include <objc/objc.h>
 #include <objc/message.h>
 #define OBJC(X, Y) ((objc_object* (*) (id, SEL)) objc_msgSend) ((objc_object*) X, sel_registerName(Y))

bool dockClickHandler(id self, SEL _cmd, ...)
{
    Q_UNUSED(self);
    Q_UNUSED(_cmd);
    ((Application*) qApp)->onClickOnDock();
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

    setWindowIcon(QIcon(":/icon.png"));

#ifdef Q_OS_MAC
    setQuitOnLastWindowClosed(false);

    Class       cls      = (Class) objc_getClass("NSApplication");
    objc_object *appInst = OBJC(cls, "sharedApplication");

    if (appInst != NULL) {
        objc_object *delegate = OBJC(appInst, "delegate");
        objc_object *delClass = OBJC(delegate, "class");

        class_addMethod((Class) delClass, sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:"), (IMP) dockClickHandler, "B@:");
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
