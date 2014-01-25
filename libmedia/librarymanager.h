#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <QObject>
#include <QStringList>

class LibraryManagerPrivate;

class LibraryManager : public QObject
{
    Q_OBJECT

public:
    static LibraryManager *instance();
    static QStringList searchPaths();

    void setSearchPaths(QStringList);

    unsigned long long countImage();
    unsigned long long countMusic();
    unsigned long long countVideo();

protected:
    LibraryManager();
    ~LibraryManager();

private:
    LibraryManagerPrivate *d;

public slots:
    void startScan();
    void stopScan();

private slots:
    void aboutToQuit();

signals:
    void processingFile(QString, unsigned long long, unsigned long long);
    void scanningFolder(QString);
    void scanFinished();
    void scanStarted();
};
#endif // LIBRARYMANAGER_H
