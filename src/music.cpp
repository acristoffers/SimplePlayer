#include "src/music.h"

struct MusicPrivate
{
};

Music::Music(QUrl file) :
    Media(file),
    d(new MusicPrivate)
{
}

Music::~Music()
{
    delete d;
}
