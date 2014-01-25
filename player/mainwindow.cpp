#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>

#include <image.h>
#include <librarymanager.h>
#include <music.h>
#include <video.h>

#include "musictreeitemmodel.h"
#include "playlistmodel.h"
#include "settings.h"
#include "videowidget.h"

struct MainWindowPrivate
{
    Ui::MainWindow *ui;

    QMediaPlayer       *player;
    QMediaPlaylist     *playlist;
    VideoWidget        *videoWidget;
    PlaylistModel      *playlistModel;
    MusicTreeItemModel *musicModel;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      d(new MainWindowPrivate)
{
    d->ui = new Ui::MainWindow;
    d->ui->setupUi(this);

    d->player        = new QMediaPlayer;
    d->playlist      = new QMediaPlaylist;
    d->videoWidget   = new VideoWidget;
    d->playlistModel = new PlaylistModel;
    d->musicModel    = new MusicTreeItemModel;

    d->playlistModel->setPlaylist(d->playlist);
    d->ui->playlist->setModel(d->playlistModel);

    d->ui->musicTree->setModel(d->musicModel);

    d->player->setPlaylist(d->playlist);
    d->player->setVideoOutput(d->videoWidget);

    connect( d->ui->actionAbout_Qt,      SIGNAL( triggered() ),                                                     qApp,             SLOT( aboutQt() ) );

    connect( LibraryManager::instance(), SIGNAL( processingFile(QString, unsigned long long, unsigned long long) ), this,             SLOT( statusForProcessingFile(QString, unsigned long long, unsigned long long) ) );
    connect( LibraryManager::instance(), SIGNAL( scanFinished() ),                                                  d->ui->statusBar, SLOT( clearMessage() ) );
    connect( LibraryManager::instance(), SIGNAL( scanFinished() ),                                                  this,             SLOT( updateLibraries() ) );
}

MainWindow::~MainWindow()
{
    d->player->deleteLater();
    d->playlist->deleteLater();
    d->playlistModel->deleteLater();
    d->videoWidget->deleteLater();

    delete d->ui;
    delete d;
}

void MainWindow::updateLibraries()
{
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
