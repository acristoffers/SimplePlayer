#ifndef VIDEO_H
#define VIDEO_H

#include "media.h"

struct VideoPrivate;

class Video : public Media
{
    Q_OBJECT

public:
    Video(QUrl file);
    ~Video();

private:
    VideoPrivate *d;
};
#endif // VIDEO_H
