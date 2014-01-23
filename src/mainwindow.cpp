#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "librarymanager.h"
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

    connect( d->ui->actionAbout_Qt,      SIGNAL( triggered() ),                                                     qApp,             SLOT( aboutQt() ) );

    connect( LibraryManager::instance(), SIGNAL( processingFile(QString, unsigned long long, unsigned long long) ), this,             SLOT( statusForProcessingFile(QString, unsigned long long, unsigned long long) ) );
    connect( LibraryManager::instance(), SIGNAL( scanFinished() ),                                                  d->ui->statusBar, SLOT( clearMessage() ) );
}

MainWindow::~MainWindow()
{
    delete d->ui;
    delete d;
}

void MainWindow::statusForProcessingFile(QString file, unsigned long long count, unsigned long long total)
{
    d->ui->statusBar->showMessage("Processing file (" + QString::number(count) + "/" + QString::number(total) + ") " + file);
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
