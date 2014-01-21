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
    void scan();
    void stopScan();

protected:
    LibraryManager();
    ~LibraryManager();
private:
    static LibraryManager *_self;
    LibraryManagerPrivate *d;
protected slots:
    void scanFolders();
    void stopped();
    void cancelScan();

signals:
    void scanStarted();
    void scanFinished();
};
#endif // LIBRARYMANAGER_H
