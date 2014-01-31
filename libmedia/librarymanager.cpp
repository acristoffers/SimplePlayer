#include "librarymanager.h"

#include <QDebug>

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QDirIterator>
#include <QSettings>
#include <QThread>

#include "database.h"
#include "librarymanagerprivate.h"

QThread               *_thread;
LibraryManagerPrivate *_worker;

LibraryManager *LibraryManager::instance()
{
    if ( LibraryManagerPrivate::_self == nullptr ) {
        LibraryManagerPrivate::_self = new LibraryManager;
    }

    return LibraryManagerPrivate::_self;
}

LibraryManager::LibraryManager() :
    QObject(0),
    d(new LibraryManagerPrivate)
{
    _thread = new QThread;
    _worker = new LibraryManagerPrivate;

    _worker->moveToThread(_thread);
    _thread->start();

    d->watcher = new QFileSystemWatcher;

    connect( _worker,    SIGNAL( processingFile(QString, unsigned long long, unsigned long long) ), this,    SIGNAL( processingFile(QString, unsigned long long, unsigned long long) ) );
    connect( _worker,    SIGNAL( scanFinished() ),                                                  this,    SIGNAL( scanFinished() ) );
    connect( _worker,    SIGNAL( scanningFolder(QString) ),                                         this,    SIGNAL( scanningFolder(QString) ) );
    connect( _worker,    SIGNAL( scanStarted() ),                                                   this,    SIGNAL( scanStarted() ) );

    connect( _thread,    SIGNAL( finished() ),                                                      _worker, SLOT( deleteLater() ) );
    connect( _thread,    SIGNAL( finished() ),                                                      _thread, SLOT( deleteLater() ) );

    connect( d->watcher, SIGNAL( directoryChanged(QString) ),                                       _worker, SLOT( folderScan(QString) ) );
    connect( d->watcher, SIGNAL( fileChanged(QString) ),                                            _worker, SLOT( fileScan(QString) ) );

    connect( qApp,       SIGNAL( aboutToQuit() ),                                                   this,    SLOT( aboutToQuit() ) );

    d->updateWatcher();
}

LibraryManager::~LibraryManager()
{
    QMetaObject::invokeMethod( _worker, SLOT( stop() ) );

    _thread->quit();
    _thread->wait();

    delete d;
}

QStringList LibraryManager::searchPaths()
{
    return QSettings().value("paths").toStringList();
}

void LibraryManager::setSearchPaths(QStringList paths)
{
    paths.removeDuplicates();

    QStringList pathsToAdd;

    for ( QString path : paths ) {
        QString shortestPath;

        for ( QString p : paths ) {
            if ( path.startsWith(p) && ( ( p.size() < shortestPath.size() ) || shortestPath.isEmpty() ) ) {
                shortestPath = p;
            }
        }

        pathsToAdd << shortestPath;
    }

    pathsToAdd.removeDuplicates();

    QSettings().setValue("paths", pathsToAdd);

    d->updateWatcher();
}

unsigned long long LibraryManager::countImage()
{
    return DataBase::instance()->countType("Image");
}

unsigned long long LibraryManager::countMusic()
{
    return DataBase::instance()->countType("Music");
}

unsigned long long LibraryManager::countVideo()
{
    return DataBase::instance()->countType("Video");
}

void LibraryManager::startScan()
{
    _worker->stop();
    QMetaObject::invokeMethod(_worker, "fullScan");
}

void LibraryManager::stopScan()
{
    QMetaObject::invokeMethod(_worker, "stop");
}

void LibraryManager::aboutToQuit()
{
    _worker->stop();
    _thread->eventDispatcher()->processEvents(QEventLoop::AllEvents);
    _thread->quit();
}
