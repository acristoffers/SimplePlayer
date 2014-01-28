#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractItemModel>

struct ImageModelPrivate;

class ImageModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ImageModel();
    ~ImageModel();

private:
    ImageModelPrivate *d;

    // QAbstractItemModel interface

public slots:
    void reloadData();

public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    bool hasChildren(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QModelIndex parent(const QModelIndex &child) const;
    int columnCount(const QModelIndex &parent) const;
};
#endif // IMAGEMODEL_H
