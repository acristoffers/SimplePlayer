#include "database.h"

#include <QDebug>
#include <QSqlError>
#include <QTimer>

#include <QDateTime>
#include <QDir>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QThread>
#include <QVariant>

#include "media.h"
#include "music.h"

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

DataBase::DataBase()
    : QObject(0),
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
        m->deleteLater();
        return;
    }

    QFileInfo info( m->file() );
    qint64    lastModified = info.lastModified().currentMSecsSinceEpoch();
    qint64    size         = info.size();

    if ( size == 0 ) {
        QThread::currentThread()->sleep(1);
        save(Media::specializedObjectForFile( m->file() ), basePath);
        m->deleteLater();
        return;
    }

    QMutexLocker locker(&mutex);

    d->query->prepare("SELECT id, size, last_modified FROM media WHERE file=?");
    d->query->addBindValue( info.canonicalFilePath() );
    d->query->exec();

    if ( !d->query->next() ) {
        d->query->prepare("INSERT INTO media(type, base_path, file, size, last_modified) VALUES(?, ?, ?, ?, ?)");
        d->query->addBindValue( m->isA() );
        d->query->addBindValue(basePath);
        d->query->addBindValue( info.canonicalFilePath() );
        d->query->addBindValue(size);
        d->query->addBindValue(lastModified);
        d->query->exec();

        if ( m->isA() == "Music" ) {
            d->query->exec("SELECT id FROM media WHERE file=\"" + info.canonicalFilePath() + "\"");

            if ( d->query->next() ) {
                int id = d->query->value("id").toInt();

                Music *mu = static_cast<Music *> (m);

                d->query->prepare("INSERT INTO music(media, artist, album, title, track, year) VALUES (?, ?, ?, ?, ?, ?)");
                d->query->addBindValue(id);
                d->query->addBindValue( mu->artist() );
                d->query->addBindValue( mu->album() );
                d->query->addBindValue( mu->title() );
                d->query->addBindValue( mu->track() );
                d->query->addBindValue( mu->year() );
                d->query->exec();
            }
        }
    } else {
        int    mid = d->query->value("id").toInt();
        qint64 mlm = d->query->value("last_modified").toLongLong();
        qint64 msz = d->query->value("size").toLongLong();

        if ( (mlm != lastModified) || (msz != size) ) {
            if ( m->isA() == "Music" ) {
                Music *mu = static_cast<Music *> (m);

                d->query->prepare("UPDATE music SET artist=?, album=?, title=?, track=?, year=? WHERE media=?");
                d->query->addBindValue( mu->artist() );
                d->query->addBindValue( mu->album() );
                d->query->addBindValue( mu->title() );
                d->query->addBindValue( mu->track() );
                d->query->addBindValue( mu->year() );
                d->query->addBindValue(mid);
                d->query->exec();
            }

            d->query->prepare("UPDATE media SET base_path=?, size=?, last_modified=? WHERE id=?");
            d->query->addBindValue(basePath);
            d->query->addBindValue(size);
            d->query->addBindValue(lastModified);
            d->query->addBindValue(mid);
            d->query->exec();
        }
    }

    m->deleteLater();
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
            d->query->prepare("SELECT id FROM media WHERE base_path=? OR base_path is null");
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

            d->query->prepare("DELETE FROM media WHERE base_path=? OR base_path is null");
            d->query->addBindValue(basePath);
            d->query->exec();
        }
    }

    d->query->exec("SELECT id, file FROM media");

    QList<int> ids;

    while ( d->query->next() ) {
        int     id   = d->query->value("id").toInt();
        QString file = d->query->value("file").toString();

        if ( !QFileInfo::exists(file) ) {
            ids << id;
        }
    }

    for ( int id : ids ) {
        d->query->prepare("DELETE FROM media WHERE id=?");
        d->query->addBindValue(id);
        d->query->exec();

        d->query->prepare("DELETE FROM music WHERE media=?");
        d->query->addBindValue(id);
        d->query->exec();
    }
}

QStringList DataBase::albumForArtist(QString artist)
{
    QMutexLocker locker(&mutex);

    d->query->prepare("SELECT DISTINCT album FROM music WHERE artist=?");
    d->query->addBindValue(artist);
    d->query->exec();

    QStringList albums;

    while ( d->query->next() ) {
        albums << d->query->value("album").toString();
    }

    return albums;
}

QStringList DataBase::allArtists()
{
    QMutexLocker locker(&mutex);

    d->query->exec("SELECT DISTINCT artist FROM music");
    QStringList artists;
    while ( d->query->next() ) {
        artists << d->query->value("artist").toString();
    }
    return artists;
}

unsigned long long DataBase::countType(QString w)
{
    QMutexLocker locker(&mutex);

    d->query->prepare("SELECT COUNT(id) AS count FROM media WHERE type=?");
    d->query->addBindValue(w);
    d->query->exec();

    if ( d->query->next() ) {
        return d->query->value("count").toULongLong();
    }

    return 0;
}

QList<Music *> DataBase::musicWhere(QMap<Fields, QVariant> where)
{
    if ( where.isEmpty() ) {
        mutex.lock();
        d->query->exec("SELECT media FROM music");

        QList<int> ids;
        while ( d->query->next() ) {
            ids << d->query->value("media").toInt();
        }
        mutex.unlock();

        QList<Music *> musics;
        for ( int id : ids ) {
            musics << new Music(id);
        }

        return musics;
    }

    QString whereSQL;

    QMapIterator<Fields, QVariant> i(where);
    while ( i.hasNext() ) {
        i.next();
        Fields   field = i.key();
        QVariant value = i.value();

        if ( !whereSQL.isEmpty() ) {
            whereSQL += " and ";
        }

        switch ( field ) {
            case Artist:
                whereSQL += "artist=\"" + value.toString() + "\"";
                break;

            case Album:
                whereSQL += "album=\"" + value.toString() + "\"";
                break;

            case Title:
                whereSQL += "title=\"" + value.toString() + "\"";
                break;

            case Track:
                whereSQL += "track=\"" + QString::number( value.toInt() ) + "\"";
                break;

            case Year:
                whereSQL += "year=\"" + QString::number( value.toInt() ) + "\"";
        }
    }

    mutex.lock();

    d->query->exec("SELECT media FROM music WHERE " + whereSQL + " ORDER BY track, title");

    QList<int> ids;
    while ( d->query->next() ) {
        ids << d->query->value("media").toInt();
    }

    mutex.unlock();

    QList<Music *> musics;
    for ( int id : ids ) {
        musics << new Music(id);
    }

    return musics;
}

bool DataBase::musicInfoForID(int id, QString *file, QString *artist, QString *album, QString *title, int *track, int *year)
{
    QMutexLocker locker(&mutex);

    d->query->prepare("SELECT file FROM media WHERE id=?");
    d->query->addBindValue(id);
    d->query->exec();

    if ( d->query->next() ) {
        *file = d->query->value("file").toString();
    } else {
        return false;
    }

    d->query->prepare("SELECT * FROM music WHERE media=?");
    d->query->addBindValue(id);
    d->query->exec();

    if ( d->query->next() ) {
        *artist = d->query->value("artist").toString();
        *album  = d->query->value("album").toString();
        *title  = d->query->value("title").toString();
        *track  = d->query->value("track").toInt();
        *year   = d->query->value("year").toInt();
    } else {
        return false;
    }

    return true;
}

bool DataBase::musicInfoForFile(QString file, QString *artist, QString *album, QString *title, int *track, int *year)
{
    QMutexLocker locker(&mutex);

    d->query->prepare("SELECT id FROM media WHERE file=?");
    d->query->addBindValue(file);
    d->query->exec();

    int id;

    if ( d->query->next() ) {
        id = d->query->value("id").toInt();
    } else {
        return false;
    }

    d->query->prepare("SELECT * FROM music WHERE media=?");
    d->query->addBindValue(id);
    d->query->exec();

    if ( d->query->next() ) {
        *artist = d->query->value("artist").toString();
        *album  = d->query->value("album").toString();
        *title  = d->query->value("title").toString();
        *track  = d->query->value("track").toInt();
        *year   = d->query->value("year").toInt();
    } else {
        return false;
    }

    return true;
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
