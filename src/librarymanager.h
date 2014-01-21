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

protected:
    LibraryManager();
    ~LibraryManager();
private:
    static LibraryManager *_self;
    LibraryManagerPrivate *d;
protected slots:
    void scanFolders();
    void stopped();

signals:
    void startScan();
    void finishedScan();
};
#endif // LIBRARYMANAGER_H
