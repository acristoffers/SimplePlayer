#include "librarymanager.h"

#include <QDebug>

#include <QDirIterator>
#include <QSettings>
#include <QThread>

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
    QSettings().setValue("paths", paths);

    d->updateWatcher();
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
