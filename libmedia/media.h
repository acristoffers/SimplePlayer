#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include <QPixmap>

struct MediaPrivate;

class Media : public QObject
{
    Q_OBJECT

public:
    QString file();
    bool isValid();

    virtual QString presentableName() = 0;
    virtual QPixmap snapshot()        = 0;

    virtual QString isA();                                // returns Image, Music, Video or NaM
    static Media *specializedObjectForFile(QString file); // returns a Image*, Music* or Video* for the given file. nullptr if NaM

protected:
    Media(QString file);
    ~Media();

    void setFile(QString);

private:
    MediaPrivate *d;
};
#endif // MEDIA_H
