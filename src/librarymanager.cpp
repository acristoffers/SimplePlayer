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

    volatile bool cancelFlag;

    void recurse(QDir, QString);
};

LibraryManager::LibraryManager() :
    QObject(0),
    d(new LibraryManagerPrivate)
{
    d->thread     = nullptr;
    d->man        = nullptr;
    d->cancelFlag = false;
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

    delete d;
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
    connect( this,   SIGNAL( scanStarted() ),    d->man, SLOT( scanFolders() ) );

    d->thread->start();
    emit scanStarted();
}

void LibraryManager::stopScan()
{
    d->man->cancelScan();
    d->thread->quit();
}

void LibraryManager::scanFolders()
{
    qDebug() << "running";
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

void LibraryManager::cancelScan()
{
    d->cancelFlag = true;
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
            } else {
                qDebug() << "NaM: " << entry.canonicalFilePath();
            }
        }
    }
}
