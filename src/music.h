#ifndef MUSIC_H
#define MUSIC_H

#include "media.h"

struct MusicPrivate;

class Music : public Media
{
    Q_OBJECT
public:
    Music(QUrl file);
    ~Music();
private:
    MusicPrivate *d;
};
#endif // MUSIC_H
