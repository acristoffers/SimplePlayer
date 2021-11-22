#ifndef MUSICTREEITEMMODEL_H
#define MUSICTREEITEMMODEL_H

#include <QAbstractItemModel>

struct MusicTreeItemModelPrivate;

class MusicTreeItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    MusicTreeItemModel(QObject *parent = 0);
    ~MusicTreeItemModel();

    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    QModelIndex parent(const QModelIndex &child) const;
    Qt::DropActions supportedDragActions() const;
    int rowCount(const QModelIndex &parent) const;
public slots:
    void reloadData();
private:
    MusicTreeItemModelPrivate *d;
};
#endif // MUSICTREEITEMMODEL_H
