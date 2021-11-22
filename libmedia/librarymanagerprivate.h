#ifndef LIBRARYMANAGERPRIVATE_H
#define LIBRARYMANAGERPRIVATE_H

#include <QFileSystemWatcher>
#include <QObject>
#include <QThread>

#include "librarymanager.h"

class LibraryManagerPrivate : public QObject
{
    Q_OBJECT
public:
    LibraryManagerPrivate();
    ~LibraryManagerPrivate();

    void updateWatcher();

    static LibraryManager *_self;
    QFileSystemWatcher *watcher;
private:
    volatile bool cancel;
    bool mustClean;

    unsigned long long totalFiles;
    unsigned long long currentFile;
public slots:
    void fileScan(QString);
    void folderScan(QString);
    void fullScan();
    void stop();
signals:
    void processingFile(QString, unsigned long long, unsigned long long);
    void scanningFolder(QString);
    void scanFinished();
    void scanStarted();
};
#endif
