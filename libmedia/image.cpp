#include "image.h"

#include <QFileInfo>

#include "database.h"

struct ImagePrivate
{
    QPixmap snapshot;
    bool    snapshot_set;
};

Image::Image(QString file)
    : Media(file),
      d(new ImagePrivate)
{
    d->snapshot_set = false;
}

Image::~Image()
{
    delete d;
}

QString Image::presentableName()
{
    return QFileInfo( file() ).canonicalFilePath();
}

QPixmap Image::snapshot()
{
    if ( !d->snapshot_set ) {
        d->snapshot     = QPixmap( file() ).scaledToWidth(128);
        d->snapshot_set = true;
    }

    return d->snapshot;
}

QList<Image *> Image::allImages()
{
    return DataBase::instance()->allImages();
}
