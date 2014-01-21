#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settings.h"

struct MainWindowPrivate
{
    Ui::MainWindow *ui;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d(new MainWindowPrivate)
{
    d->ui = new Ui::MainWindow;
    d->ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete d->ui;
    delete d;
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionSettings_triggered()
{
    Settings *s = new Settings(this);

    s->show();
}

void MainWindow::on_actionAdd_Path_triggered()
{
    Settings *s = new Settings(this);

    s->show();
    s->requestToAddPath();
}
