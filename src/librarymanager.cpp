#include "src/librarymanager.h"

#include <QDebug>

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QMimeDatabase>
#include <QMimeType>
#include <QSettings>
#include <QStringList>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include "database.h"
#include "media.h"

LibraryManager *LibraryManager::_self = nullptr;

LibraryManager *LibraryManager::instance()
{
    if ( _self == nullptr ) {
        _self = new LibraryManager;
    }
    return _self;
}

struct LibraryManagerPrivate
{
    volatile bool  cancelFlag;
    LibraryManager *man;
    QThread        *thread;

    QFileSystemWatcher *watcher;

    void        recurse(QDir, QString);
    QStringList subfoldersList(QString);
};

LibraryManager::LibraryManager() :
    QObject(0),
    d(new LibraryManagerPrivate)
{
    d->thread     = nullptr;
    d->man        = nullptr;
    d->cancelFlag = false;

    d->watcher = new QFileSystemWatcher;

    connect( d->watcher, SIGNAL( directoryChanged(QString) ), this, SLOT( rescanFolder(QString) ) );

    changeWatchPaths();
}

LibraryManager::~LibraryManager()
{
    if ( d->thread ) {
        stopScan();
        d->thread->wait();
        d->thread->deleteLater();
    }

    if ( d->man ) {
        d->man->deleteLater();
    }

    d->watcher->removePaths( d->watcher->directories() );
    d->watcher->deleteLater();

    delete d;
}

void LibraryManager::changeWatchPaths()
{
    d->watcher->removePaths( d->watcher->directories() );
    QStringList paths = QSettings().value("paths").toStringList();
    for ( QString path : paths ) {
        d->watcher->addPaths( d->subfoldersList(path) );
    }
}

void LibraryManager::scan()
{
    if ( d->thread == nullptr ) {
        d->thread = new QThread;
        d->man    = new LibraryManager;
    }

    if ( d->thread->isRunning() ) {
        stopScan();
        QTimer::singleShot( 1000, this, SLOT( scan() ) );
        return;
    }

    d->man->deleteLater();
    d->thread->deleteLater();

    d->thread = new QThread;
    d->man    = new LibraryManager;

    d->man->moveToThread(d->thread);

    connect( d->man, SIGNAL( scanFinished() ), this,   SLOT( stopped() ) );
    connect( this,   SIGNAL( scanStarted() ),  d->man, SLOT( scanFolders() ) );

    d->thread->start();
    emit scanStarted();
}

void LibraryManager::stopScan()
{
    d->man->cancelScan();
    d->thread->quit();
}

void LibraryManager::cancelScan()
{
    d->cancelFlag = true;
}

void LibraryManager::rescanFolder(QString folder)
{
    if ( QThread::currentThread() == qApp->thread() ) {
        if ( d->thread == nullptr ) {
            d->thread = new QThread;
            d->man    = new LibraryManager;
        }

        if ( d->thread->isRunning() ) {
            stopScan();
            QTimer::singleShot( 1000, this, SLOT( scan() ) );
            return;
        }

        d->man->deleteLater();
        d->thread->deleteLater();

        d->thread = new QThread;
        d->man    = new LibraryManager;

        d->man->moveToThread(d->thread);

        connect( d->man, SIGNAL( scanFinished() ),       this,   SLOT( stopped() ) );
        connect( this,   SIGNAL( scanStarted(QString) ), d->man, SLOT( rescanFolder(QString) ) );

        d->thread->start();
        emit scanStarted(folder);
    } else {
        DataBase::instance()->clean();

        QStringList paths = QSettings().value("paths").toStringList();
        QString     base;

        for ( QString path : paths ) {
            QStringList subdirs = d->subfoldersList(path);
            for ( QString subdir : subdirs ) {
                if ( QDir(folder).canonicalPath() == QDir(subdir).canonicalPath() ) {
                    base = path;
                }
            }
        }

        d->recurse(folder, base);
    }
}

void LibraryManager::scanFolders()
{
    DataBase::instance()->clean();

    QSettings   settings;
    QStringList paths = settings.value("paths").toStringList();

    for ( auto path : paths ) {
        d->recurse( path, QDir(path).canonicalPath() );

        if ( d->cancelFlag ) {
            break;
        }
    }

    emit scanFinished();
}

void LibraryManager::stopped()
{
    d->thread->quit();
    d->thread->wait();
}

void LibraryManagerPrivate::recurse(QDir path, QString basePath)
{
    for ( QFileInfo entry : path.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot) ) {
        if ( cancelFlag ) {
            return;
        }

        if ( entry.isDir() ) {
            recurse(entry.absoluteFilePath(), basePath);
        } else {
            Media *m = Media::specializedObjectForFile( entry.canonicalFilePath() );

            if ( m != nullptr ) {
                DataBase::instance()->save(m, basePath);
            }
        }
    }
}

QStringList LibraryManagerPrivate::subfoldersList(QString dir)
{
    QStringList  sl;
    QDirIterator it(dir, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    while ( it.hasNext() ) {
        sl.append( it.next() );
    }

    return sl;
}
