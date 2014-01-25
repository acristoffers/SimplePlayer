#include "music.h"

#include <QFileInfo>
#include <QImage>
#include <QMimeDatabase>
#include <QPixmap>
#include <QUrl>

#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/tbytevectorlist.h>

#include "database.h"

struct MusicPrivate
{
    QPixmap cover;

    QString album;
    QString artist;
    QString title;

    int track;
    int year;
};

Music::Music(QString file, bool preferDataBaseData)
    : Media(file),
      d(new MusicPrivate)
{
    if ( preferDataBaseData ) {
        QString artist, album, title;
        int     track, year;
        bool    b = DataBase::instance()->musicInfoForFile(file, &artist, &album, &title, &track, &year);
        if ( b ) {
            d->artist = artist;
            d->album  = album;
            d->title  = title;
            d->track  = track;
            d->year   = year;
            return;
        }
    }

    TagLib::FileRef f( QFileInfo(file).canonicalFilePath().toLocal8Bit().data() );

    if ( !f.isNull() ) {
        d->artist = QString::fromUtf8( f.tag()->artist().to8Bit(true).data() );
        d->album  = QString::fromUtf8( f.tag()->album().to8Bit(true).data() );
        d->title  = QString::fromUtf8( f.tag()->title().to8Bit(true).data() );
        d->track  = f.tag()->track();
        d->year   = f.tag()->year();
    }
}

Music::Music(QString file, QString artist, QString album, QString title, int track, int year)
    : Media(file),
      d(new MusicPrivate)
{
    d->artist = artist;
    d->album  = album;
    d->title  = title;
    d->track  = track;
    d->year   = year;
}

Music::~Music()
{
    delete d;
}

QString Music::presentableName()
{
    return title();
}

QPixmap Music::snapshot()
{
    return cover();
}

QStringList Music::artists()
{
    return DataBase::instance()->allArtists();
}

QStringList Music::albumsForArtist(QString artist)
{
    return DataBase::instance()->albumForArtist(artist);
}

QList<Music *> Music::titlesForArtistAndAlbum(QString artist, QString album)
{
    QMap<Fields, QVariant> map;
    map[Artist] = artist;
    map[Album]  = album;
    return DataBase::instance()->musicWhere(map);
}

QString Music::album()
{
    return d->album;
}

QString Music::artist()
{
    return d->artist;
}

QString Music::title()
{
    return d->title.isEmpty() ? file() : d->title;
}

int Music::track()
{
    return d->track;
}

int Music::year()
{
    return d->year;
}

QPixmap Music::cover()
{
    if ( d->cover.isNull() ) {
        QMimeDatabase db;
        QMimeType     type = db.mimeTypeForUrl( QUrl::fromLocalFile( file() ) );
        if ( "audio/mpeg" == type.name() ) {
            TagLib::MPEG::File audioFile( file().toStdString().c_str() );

            QByteArray cover, other;

            TagLib::ID3v2::Tag *tag = audioFile.ID3v2Tag(true);
            if ( tag ) {
                static const char *idPicture = "APIC";

                TagLib::ID3v2::FrameList            frames;
                TagLib::ID3v2::AttachedPictureFrame *apf;

                frames = tag->frameListMap()[idPicture];

                if ( !frames.isEmpty() ) {
                    for ( auto frame : frames ) {
                        apf = (TagLib::ID3v2::AttachedPictureFrame *) frame;

                        if ( apf->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover ) {
                            cover = QByteArray( apf->picture().data(), apf->picture().size() );
                            break;
                        } else {
                            other = QByteArray( apf->picture().data(), apf->picture().size() );
                        }
                    }
                }
            }

            if ( cover.isEmpty() ) {
                d->cover = QPixmap::fromImage( QImage::fromData(other) );
            } else {
                d->cover = QPixmap::fromImage( QImage::fromData(cover) );
            }
        }
    }

    return d->cover;
}

Music::Music(int id)
    : Media(""),
      d(new MusicPrivate)
{
    QString file, artist, album, title;
    int     track, year;

    if ( DataBase::instance()->musicInfoForID(id, &file, &artist, &album, &title, &track, &year) ) {
        setFile(file);
        d->artist = artist;
        d->album  = album;
        d->title  = title;
        d->track  = track;
        d->year   = year;
    }
}
