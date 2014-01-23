#ifndef IMAGE_H
#define IMAGE_H

#include "media.h"

struct ImagePrivate;

class Image : public Media
{
    Q_OBJECT

public:
    Image(QUrl file);
    ~Image();

private:
    ImagePrivate *d;
};
#endif // IMAGE_H
