#include "src/librarymanager.h"

#include <QDebug>

#include <QDir>
#include <QFileInfo>
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
    QThread        *thread;
    LibraryManager *man;

    void recurse(QDir, QString);
};

LibraryManager::LibraryManager() :
    QObject(0),
    d(new LibraryManagerPrivate)
{
    d->thread = nullptr;
    d->man    = nullptr;
}

LibraryManager::~LibraryManager()
{
    if ( d->thread ) {
        d->thread->quit();
        d->thread->wait();
        d->thread->deleteLater();
    }

    if ( d->man ) {
        d->man->deleteLater();
    }

    delete d;
}

void LibraryManager::scan()
{
    if ( d->thread == nullptr ) {
        d->thread = new QThread;
        d->man    = new LibraryManager;
    }

    if ( d->thread->isRunning() ) {
        d->thread->quit();
        QTimer::singleShot( 1000, this, SLOT( scan() ) );
        return;
    }

    d->man->deleteLater();
    d->thread->deleteLater();

    d->thread = new QThread;
    d->man    = new LibraryManager;

    d->man->moveToThread(d->thread);

    connect( d->man, SIGNAL( finishedScan() ), this,   SLOT( stopped() ) );
    connect( this,   SIGNAL( startScan() ),    d->man, SLOT( scanFolders() ) );

    d->thread->start();
    emit startScan();
}

void LibraryManager::scanFolders()
{
    qDebug() << "running";
    DataBase::instance()->clean();

    QSettings   settings;
    QStringList paths = settings.value("paths").toStringList();

    for ( auto path : paths ) {
        d->recurse( path, QDir(path).canonicalPath() );
    }

    emit finishedScan();
}

void LibraryManager::stopped()
{
    d->thread->quit();
    d->thread->wait();
}

void LibraryManagerPrivate::recurse(QDir path, QString basePath)
{
    for ( QFileInfo entry : path.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot) ) {
        if ( entry.isDir() ) {
            recurse(entry.absoluteFilePath(), basePath);
        } else {
            Media *m = Media::specializedObjectForFile( entry.canonicalFilePath() );

            if ( m != nullptr ) {
                DataBase::instance()->save(m, basePath);
            } else {
                qDebug() << "NaM: " << entry.canonicalFilePath();
            }
        }
    }
}
