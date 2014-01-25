#ifndef IMAGE_H
#define IMAGE_H

#include "media.h"

struct ImagePrivate;

class Image : public Media
{
    Q_OBJECT

public:
    Image(QString file);
    ~Image();

    QString presentableName() override;
    QPixmap snapshot() override;

private:
    ImagePrivate *d;
};
#endif // IMAGE_H
