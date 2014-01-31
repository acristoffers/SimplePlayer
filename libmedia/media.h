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

    virtual QString isA();
    static Media *specializedObjectForFile(QString file);

protected:
    Media(QString file);
    ~Media();

    void setFile(QString);

private:
    MediaPrivate *d;
};
#endif // MEDIA_H
