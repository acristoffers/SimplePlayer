#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

struct DataBasePrivate;
class Media;

class DataBase : public QObject
{
    Q_OBJECT
public:
    static DataBase *instance();

    void save(Media *, QString basePath);
    void clean();

protected:
    DataBase();
    ~DataBase();
private:
    DataBasePrivate *d;
    static DataBase *_self;
};
#endif // DATABASE_H
