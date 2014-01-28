#ifndef VIDEOMODEL_H
#define VIDEOMODEL_H

#include <QAbstractItemModel>

struct VideoModelPrivate;

class VideoModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    VideoModel();
    ~VideoModel();

private:
    VideoModelPrivate *d;

    // QAbstractItemModel interface

public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    bool hasChildren(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
};
#endif // VIDEOMODEL_H
