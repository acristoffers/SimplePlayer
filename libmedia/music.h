#ifndef MUSIC_H
#define MUSIC_H

#include "media.h"

struct MusicPrivate;

class Music : public Media
{
    Q_OBJECT

public:
    Music(QString file, bool preferDataBaseData = true);
    Music(QString file, QString artist, QString album, QString title, int track, int year);

    ~Music();

    QString presentableName() override;
    QPixmap snapshot() override;

    static QStringList artists();

    static QStringList    albumsForArtist(QString);
    static QList<Music *> titlesForArtistAndAlbum(QString, QString);

    QString album();
    QString artist();
    QString title();
    int track();
    int year();
    virtual QPixmap cover();

protected:
    Music(int id);

private:
    MusicPrivate *d;

    friend class DataBase;
};
#endif // MUSIC_H
