#include "image.h"

#include <QFileInfo>

struct ImagePrivate
{
};

Image::Image(QString file)
    : Media(file),
      d(new ImagePrivate)
{
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
    return QPixmap();
}
