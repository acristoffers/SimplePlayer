#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include <QUrl>

struct MediaPrivate;

class Media : public QObject
{
    Q_OBJECT
public:
    QUrl file();
    bool isValid();

    QString album();
    QString artist();
    QString title();
    int track();
    int year();

    virtual QPixmap cover();

    QString isA();                                        // returns Image, Music, Video or NaM
    static Media *specializedObjectForFile(QString file); // returns a Image*, Music* or Video* for the given file. nullptr if NaM

protected:
    Media(QUrl file);
    ~Media();
private:
    MediaPrivate *d;
};
#endif // MEDIA_H
