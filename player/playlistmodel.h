#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>

QT_BEGIN_NAMESPACE
class QMediaPlaylist;
QT_END_NAMESPACE

struct PlaylistModelPrivate;

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

public:
    PlaylistModel(QMediaPlaylist *playlist);
    ~PlaylistModel();

private:
    PlaylistModelPrivate *d;

    // QAbstractItemModel interface

public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;
    Qt::DropActions supportedDragActions() const;
    bool removeRows(QList<int> rows);
    Qt::ItemFlags flags(const QModelIndex &index) const;
};
#endif // PLAYLISTMODEL_H
