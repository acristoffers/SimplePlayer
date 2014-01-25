#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QVariant>

struct DataBasePrivate;
class Media;
class Music;

enum Fields {
    Artist = 0x0,
    Album  = 0x1,
    Title  = 0x2,
    Track  = 0x4,
    Year   = 0x6
};

class DataBase : public QObject
{
    Q_OBJECT

public:
    static DataBase *instance();

    void save(Media *, QString basePath);
    void clean();

    QStringList albumForArtist(QString);
    QStringList allArtists();

    unsigned long long countType(QString);

    QList<Music *> musicWhere(QMap<Fields, QVariant> );

    bool musicInfoForFile(QString file, QString *artist, QString *album, QString *title, int *track, int *year);
    bool musicInfoForID(int id, QString *file, QString *artist, QString *album, QString *title, int *track, int *year);

protected:
    DataBase();
    ~DataBase();

private:
    DataBasePrivate *d;
    static DataBase *_self;
};
#endif // DATABASE_H
