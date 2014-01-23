#include "librarymanagerprivate.h"

#include <QDebug>

#include <QApplication>
#include <QDirIterator>

#include "database.h"
#include "media.h"

LibraryManager *LibraryManagerPrivate::_self = nullptr;

LibraryManagerPrivate::LibraryManagerPrivate()
{
    cancel      = false;
    mustClean   = true;
    watcher     = nullptr;
    totalFiles  = 0;
    currentFile = 0;
}

LibraryManagerPrivate::~LibraryManagerPrivate()
{
    if ( watcher ) {
        delete watcher;
    }
}

void LibraryManagerPrivate::updateWatcher()
{
    QStringList paths = LibraryManager::searchPaths();

    watcher->removePaths( watcher->directories() );
    watcher->removePaths( watcher->files() );

    QStringList sl = paths;

    for ( QString path : paths ) {
        QDirIterator it(path, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        while ( it.hasNext() ) {
            sl << it.next();
        }
    }

    watcher->addPaths(sl);
}

void LibraryManagerPrivate::fileScan(QString file)
{
    if ( mustClean ) {
        DataBase::instance()->clean();
    }

    Media *m = Media::specializedObjectForFile(file);

    if ( totalFiles != 0 ) {
        currentFile++;
        emit processingFile(file, currentFile, totalFiles);
    }

    if ( m && m->isValid() ) {
        QStringList paths = LibraryManager::searchPaths();
        QString     shortestPath;

        for ( QString dpath : paths ) {
            dpath = QDir(dpath).canonicalPath();

            if ( file.startsWith(dpath) && ( ( dpath.size() < shortestPath.size() ) || shortestPath.isEmpty() ) ) {
                shortestPath = dpath;
            }
        }

        DataBase::instance()->save(m, shortestPath);
    }
}

void LibraryManagerPrivate::folderScan(QString path)
{
    cancel = false;

    emit scanningFolder(path);

    if ( mustClean ) {
        DataBase::instance()->clean();
    }

    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::FollowSymlinks);
    while ( it.hasNext() ) {
        if ( cancel ) {
            return;
        }

        fileScan( it.next() );
    }
}

void LibraryManagerPrivate::fullScan()
{
    cancel    = false;
    mustClean = false;

    totalFiles  = 0;
    currentFile = 0;

    emit scanStarted();

    DataBase::instance()->clean();

    QStringList paths = LibraryManager::searchPaths();

    for ( QString path : paths ) {
        QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
        while ( it.hasNext() ) {
            totalFiles++;
            it.next();
        }
    }

    for ( QString path : paths ) {
            folderScan(path);

        QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
        while ( it.hasNext() ) {
            if ( cancel ) {
                mustClean = true;
                emit scanFinished();
                return;
            }

            folderScan( it.next() );
        }
    }

    totalFiles  = 0;
    currentFile = 0;
    mustClean   = true;

    emit scanFinished();
}

void LibraryManagerPrivate::stop()
{
    cancel = true;
}
