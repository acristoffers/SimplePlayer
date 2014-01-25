#include "video.h"

#include <QFileInfo>

struct VideoPrivate
{
};

Video::Video(QString file)
    : Media(file),
      d(new VideoPrivate)
{
}

Video::~Video()
{
    delete d;
}

QString Video::presentableName()
{
    return QFileInfo( file() ).canonicalFilePath();
}

QPixmap Video::snapshot()
{
    return QPixmap();
}
