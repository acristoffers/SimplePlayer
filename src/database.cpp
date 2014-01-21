#include "src/database.h"

#include <QDebug>
#include <QSqlError>

#include <QDateTime>
#include <QDir>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

#include "media.h"

#define DBVERSION 1
#define xstr(s) str(s)
#define str(s)  # s

struct DataBasePrivate
{
    QSqlDatabase db;
    QSqlQuery    *query;

    QString dataFolder();
    void    open();
    void    update();
};

QMutex   mutex;
DataBase *DataBase::_self = nullptr;

DataBase *DataBase::instance()
{
    if ( _self == nullptr ) {
        _self = new DataBase;
    }
    return _self;
}

DataBase::DataBase() :
    QObject(0),
    d(new DataBasePrivate)
{
    d->open();
    d->update();
}

DataBase::~DataBase()
{
    delete d->query;
    delete d;
}

void DataBase::save(Media *m, QString basePath)
{
    if ( !m->isValid() ) {
        qDebug() << m->file().path() << " is invalid";
        return;
    }

    QFileInfo info( m->file().path() );
    qint64    lastModified = info.lastModified().currentMSecsSinceEpoch();
    qint64    size         = info.size();

    QMutexLocker locker(&mutex);

    d->query->prepare("SELECT id FROM media WHERE file=?");
    d->query->addBindValue( QFileInfo( m->file().path() ).canonicalFilePath() );
    d->query->exec();

    if ( !d->query->next() ) {
        d->query->prepare("INSERT INTO media(type, base_path, file, size, last_modified) VALUES(?, ?, ?, ?, ?)");
        d->query->addBindValue( m->isA() );
        d->query->addBindValue(basePath);
        d->query->addBindValue( m->file().path() );
        d->query->addBindValue(lastModified);
        d->query->addBindValue(size);
        d->query->exec();

        if ( m->isA() == "Music" ) {
            d->query->exec("SELECT id FROM media WHERE file=\"" + QFileInfo( m->file().path() ).canonicalFilePath() + "\"");

            if ( d->query->next() ) {
                int id = d->query->value("id").toInt();

                d->query->prepare("INSERT INTO music(media, artist, album, title, track, year) VALUES (?, ?, ?, ?, ?, ?)");
                d->query->addBindValue(id);
                d->query->addBindValue( m->artist() );
                d->query->addBindValue( m->album() );
                d->query->addBindValue( m->title() );
                d->query->addBindValue( m->track() );
                d->query->addBindValue( m->year() );
                d->query->exec();
            }
        }
    }
}

void DataBase::clean()
{
    QSettings   settings;
    QStringList paths = settings.value("paths").toStringList();

    QMutexLocker locker(&mutex);

    d->query->exec("SELECT DISTINCT base_path FROM media");

    QStringList basePaths;

    while ( d->query->next() ) {
        basePaths << d->query->value("base_path").toString();
    }

    for ( QString basePath : basePaths ) {
        bool found = false;

        for ( QString p : paths ) {
            if ( p == basePath ) {
                found = true;
            }
        }

        if ( !found ) {
            d->query->prepare("SELECT id FROM media WHERE base_path=?");
            d->query->addBindValue(basePath);
            d->query->exec();

            QList<int> ids;

            while ( d->query->next() ) {
                ids << d->query->value("id").toInt();
            }

            for ( int id : ids ) {
                d->query->prepare("DELETE FROM music WHERE media=?");
                d->query->addBindValue(id);
                d->query->exec();
            }

            d->query->prepare("DELETE FROM media WHERE base_path=?");
            d->query->addBindValue(basePath);
            d->query->exec();
        }
    }

    d->query->exec("SELECT id, file FROM media");

    while ( d->query->next() ) {
        int     id   = d->query->value("id").toInt();
        QString file = d->query->value("file").toString();

        if ( !QFileInfo::exists(file) ) {
            d->query->prepare("DELETE FROM media WHERE id=?");
            d->query->addBindValue(id);
            d->query->exec();

            d->query->prepare("DELETE FROM music WHERE media=?");
            d->query->addBindValue(id);
            d->query->exec();
        }
    }
}

QString DataBasePrivate::dataFolder()
{
    QStringList list = QStandardPaths::standardLocations(QStandardPaths::DataLocation);

    if ( list.empty() ) {
        return "";
    } else {
        return list.first();
    }
}

void DataBasePrivate::open()
{
    QDir().mkpath( dataFolder() );
    QString path = dataFolder().append( QDir::separator() ).append("SimplePlayer.db");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    query = new QSqlQuery(db);
}

void DataBasePrivate::update()
{
    QMutexLocker locker(&mutex);

    QString version = xstr(DBVERSION);

    query->exec("SELECT version FROM version");

    if ( !query->next() ) {
        query->exec("CREATE TABLE version (version INT)");
        query->exec("CREATE TABLE media (id INTEGER PRIMARY KEY AUTOINCREMENT, type TEXT, base_path TEXT, file TEXT NOT NULL, size INT, last_modified INT)");
        query->exec("CREATE TABLE music (id INTEGER PRIMARY KEY AUTOINCREMENT, media INT, artist TEXT, album TEXT, title TEXT, track INT, year INT)");
        query->exec("INSERT INTO version VALUES (" + version + ")");
    } else {
        int ver = query->value("version").toInt();
        if ( ver == DBVERSION ) {
            return;
        }
        switch ( ver ) {
            case 1:
                // add transition from DBVERSION 1 to 2
                break; // no breaks, its here because this place is empty.
                // u.s.w.
        }
    }
}
