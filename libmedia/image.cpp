#include "image.h"

struct ImagePrivate
{
};

Image::Image(QUrl file) :
    Media(file),
    d(new ImagePrivate)
{
}

Image::~Image()
{
    delete d;
}
