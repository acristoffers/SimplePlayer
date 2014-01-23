#include "video.h"

struct VideoPrivate
{
};

Video::Video(QUrl file) :
    Media(file),
    d(new VideoPrivate)
{
}

Video::~Video()
{
    delete d;
}
