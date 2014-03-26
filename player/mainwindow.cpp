#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>

#include <librarymanager.h>

#include "about.h"
#include "musictreeitemmodel.h"
#include "playercontrols.h"
#include "playlistmodel.h"
#include "settings.h"

struct MainWindowPrivate
{
    Ui::MainWindow *ui;

    QMediaPlayer       *player;
    QMediaPlaylist     *playlist;
    PlaylistModel      *playlistModel;
    MusicTreeItemModel *musicModel;
};

extern void qt_mac_set_dock_menu(QMenu *);

MainWindow *MainWindow::_self = nullptr;

MainWindow *MainWindow::instance()
{
    if ( _self == nullptr ) {
        _self = new MainWindow;
    }

    return _self;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      d(new MainWindowPrivate)
{
    d->ui = new Ui::MainWindow;
    d->ui->setupUi(this);

    d->player        = new QMediaPlayer;
    d->playlist      = new QMediaPlaylist;
    d->playlistModel = new PlaylistModel(d->playlist);
    d->musicModel    = new MusicTreeItemModel;

    d->ui->playlist->setModel(d->playlistModel);
    d->ui->musicTree->setModel(d->musicModel);

    d->player->setPlaylist(d->playlist);
    d->player->setNotifyInterval(500);

    d->ui->playercontrols->setPlayer(d->player);

    d->ui->splitter->setStretchFactor(1, 3);

    connect( d->ui->actionAbout_Qt,      SIGNAL( triggered() ),                                                     qApp,             SLOT( aboutQt() ) );
    connect( d->ui->actionAbout,         SIGNAL( triggered() ),                                                     this,             SLOT( about() ) );

    connect( LibraryManager::instance(), SIGNAL( processingFile(QString, unsigned long long, unsigned long long) ), this,             SLOT( statusForProcessingFile(QString, unsigned long long, unsigned long long) ) );
    connect( LibraryManager::instance(), SIGNAL( scanFinished() ),                                                  d->ui->statusBar, SLOT( clearMessage() ) );

    connect( d->ui->playlist,            SIGNAL( doubleClicked(QModelIndex) ),                                      d->playlistModel, SLOT( playIndex(QModelIndex) ) );
    connect( d->ui->playlist,            SIGNAL( doubleClicked(QModelIndex) ),                                      d->player,        SLOT( play() ) );

    connect( d->player,                  SIGNAL( positionChanged(qint64) ),                                         this,             SLOT( updateTextPosition(qint64) ) );

    Settings settings(this);
    while ( !settings.hasPath() ) {
        settings.show();
        settings.requestToAddPath();
    }

#ifdef Q_OS_MAC
    QMenu   *dock      = new QMenu;
    QAction *next      = new QAction(tr("Next"), dock);
    QAction *prev      = new QAction(tr("Previous"), dock);
    QAction *playpause = new QAction(tr("Play/Pause"), dock);

    connect( next,      SIGNAL( triggered() ), d->playlist,           SLOT( next() ) );
    connect( prev,      SIGNAL( triggered() ), d->playlist,           SLOT( previous() ) );
    connect( playpause, SIGNAL( triggered() ), d->ui->playercontrols, SLOT( switchPlayPause() ) );

    dock->addAction(prev);
    dock->addAction(playpause);
    dock->addAction(next);

    qt_mac_set_dock_menu(dock);
#endif
}

MainWindow::~MainWindow()
{
    d->player->deleteLater();
    d->playlist->deleteLater();
    d->playlistModel->deleteLater();

    delete d->ui;
    delete d;
}

void MainWindow::about()
{
    About *about = new About;

    about->setModal(true);
    about->show();
    about->setGeometry( QRect( about->pos(), about->sizeHint() ) );
    about->setMinimumSize( about->size() );
    about->setMaximumSize( about->size() );

    connect( about, SIGNAL( finished(int) ), about, SLOT( deleteLater() ) );
}

void MainWindow::statusForProcessingFile(QString file, unsigned long long count, unsigned long long total)
{
    d->ui->statusBar->showMessage(tr("Processing file (") + QString::number(count) + "/" + QString::number(total) + ") " + file);
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

void MainWindow::on_actionRescan_triggered()
{
    LibraryManager::instance()->startScan();
}

QString millisecondsToReadableTime(qint64 value)
{
    value /= 1000; // go to seconds

    int h, m;
    h = m = 0;

    while ( value >= 3600 ) {
        value -= 3600;
        h++;
    }

    while ( value >= 60 ) {
        value -= 60;
        m++;
    }

    QString hours, minutes, seconds;

    if ( h < 10 ) {
        hours = "0";
    }

    if ( m < 10 ) {
        minutes = "0";
    }

    if ( value < 10 ) {
        seconds = "0";
    }

    hours   += QString::number(h);
    minutes += QString::number(m);
    seconds += QString::number(value);

    QString time;

    if ( h == 0 ) {
        hours = "";
    } else {
        hours += ":";
    }

    return hours + minutes + ":" + seconds;
}

void MainWindow::updateTextPosition(qint64 current)
{
    qint64 total = d->player->duration();

    QString c, t;

    c = millisecondsToReadableTime(current);
    t = millisecondsToReadableTime(total);

    d->ui->statusBar->showMessage(c + " / " + t);
}
