#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <QObject>

struct LibraryManagerPrivate;

class LibraryManager : public QObject
{
    Q_OBJECT
public:
    static LibraryManager *instance();

public slots:
    void changeWatchPaths();
    void scan();
    void stopScan();

protected:
    LibraryManager();
    ~LibraryManager();
private:
    static LibraryManager *_self;
    LibraryManagerPrivate *d;
protected slots:
    void cancelScan();

    void rescanFolder(QString);

    void scanFolders();
    void stopped();

signals:
    void scanStarted();
    void scanFinished();

    void scanStarted(QString);
};
#endif // LIBRARYMANAGER_H
