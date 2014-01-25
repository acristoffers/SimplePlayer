#include "media.h"

#include <QDebug>

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QUrl>

#include "image.h"
#include "music.h"
#include "video.h"

struct MediaPrivate
{
    QString file;
};

Media::Media(QString file)
    : QObject(0),
      d(new MediaPrivate)
{
    d->file = file;
}

Media::~Media()
{
    delete d;
}

QString Media::file()
{
    return d->file;
}

void Media::setFile(QString file)
{
    d->file = file;
}

bool Media::isValid()
{
    return QUrl::fromLocalFile(d->file).isValid() && QFile::exists(d->file);
}

QString Media::isA()
{
    QMimeDatabase db;
    QString       mime = db.mimeTypeForUrl( QUrl::fromLocalFile(d->file) ).name();

    if ( mime.startsWith("audio/") && !d->file.endsWith(".m3u") ) {
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
    QString       mime = db.mimeTypeForUrl( QUrl::fromLocalFile(file) ).name();

    if ( mime.startsWith("audio/") && !file.endsWith(".m3u") ) {
        return new Music(file, false);
    } else if ( mime.startsWith("video/") ) {
        return new Video(file);
    } else if ( mime.startsWith("image/") ) {
        return new Image(file);
    }

    return nullptr;
}
