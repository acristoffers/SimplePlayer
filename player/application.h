#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);

    void onClickOnDock();

private:
    void aboutToQuit();
};
#endif // APPLICATION_H
