#include "media.h"

#include <QDebug>

#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QMimeDatabase>
#include <QPixmap>

#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/tbytevectorlist.h>

#include "image.h"
#include "music.h"
#include "video.h"

struct MediaPrivate
{
    QUrl file;

    QPixmap cover;

    QString album;
    QString artist;
    QString title;

    int track;
    int year;
};

Media::Media(QUrl file) :
    QObject(0),
    d(new MediaPrivate)
{
    d->file = file;

    TagLib::FileRef f( QFileInfo( file.path() ).canonicalFilePath().toLocal8Bit().data() );

    if ( !f.isNull() ) {
        d->artist = QString( f.tag()->artist().toCString() );
        d->album  = QString( f.tag()->album().toCString() );
        d->title  = QString( f.tag()->title().toCString() );
        d->track  = f.tag()->track();
        d->year   = f.tag()->year();
    }
}

Media::~Media()
{
    delete d;
}

QUrl Media::file()
{
    return d->file;
}

bool Media::isValid()
{
    return d->file.isValid() && QFile::exists( d->file.path() );
}

QString Media::album()
{
    return d->album;
}

QString Media::artist()
{
    return d->artist;
}

QString Media::title()
{
    return d->title;
}

int Media::track()
{
    return d->track;
}

int Media::year()
{
    return d->year;
}

QPixmap Media::cover()
{
    if ( d->cover.isNull() ) {
        QMimeDatabase db;
        QMimeType     type = db.mimeTypeForUrl(d->file);
        if ( "audio/mpeg" == type.name() ) {
            TagLib::MPEG::File audioFile( d->file.path().toStdString().c_str() );

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

QString Media::isA()
{
    QMimeDatabase db;
    QString       mime = db.mimeTypeForUrl(d->file).name();

    if ( mime.startsWith("audio/") ) {
        return "Music";
    } else if ( mime.startsWith("video/") ) {
        return "Video";
    } else if ( mime.startsWith("image/") ) {
        return "Image";
    }

    return "NaM";
}

Media *Media::specializedObjectForFile(QString file)
{
    QMimeDatabase db;
    QString       mime = db.mimeTypeForUrl( QUrl(file) ).name();

    if ( mime.startsWith("audio/") ) {
        return new Music( QUrl(file) );
    } else if ( mime.startsWith("video/") ) {
        return new Video( QUrl(file) );
    } else if ( mime.startsWith("image/") ) {
        return new Image( QUrl(file) );
    }

    return nullptr;
}
