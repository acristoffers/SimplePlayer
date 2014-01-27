#include "playlistmodel.h"

#include <QDebug>

#include <QByteArray>
#include <QDataStream>
#include <QMediaContent>
#include <QMediaPlaylist>
#include <QMimeData>

#include <music.h>

struct PlaylistModelPrivate
{
    QMediaPlaylist *playlist;
};

PlaylistModel::PlaylistModel(QMediaPlaylist *playlist)
    : QAbstractListModel(0),
      d(new PlaylistModelPrivate)
{
    d->playlist = playlist;
}

PlaylistModel::~PlaylistModel()
{
    d->playlist->deleteLater();
    delete d;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if ( (column == 0) && ( row <= d->playlist->mediaCount() ) ) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->playlist->mediaCount();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && (role == Qt::DisplayRole) ) {
        QString file = d->playlist->media( index.row() ).canonicalUrl().path();
        Music   m(file);
        return m.title();
    }

    return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);

    if ( role == Qt::DisplayRole ) {
        return "Title";
    } else {
        return QVariant();
    }
}

QStringList PlaylistModel::mimeTypes() const
{
    return QStringList() << "application/file-list" << "application/ids-list" << QAbstractListModel::mimeTypes();
}

QMimeData *PlaylistModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *data = new QMimeData;

    QByteArray  byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);

    for ( QModelIndex index : indexes ) {
        if ( index.isValid() ) {
            qint32 row = index.row();
            stream << row;
        }
    }

    data->setData("application/ids-list", byteArray);

    return data;
}

bool PlaylistModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);

    if ( ( (action == Qt::MoveAction) || (action == Qt::CopyAction) ) && ( data->hasFormat("application/file-list") || data->hasFormat("application/ids-list") ) ) {
        return true;
    }

    return QAbstractListModel::canDropMimeData(data, action, row, column, parent);
}

bool PlaylistModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if ( ( (action == Qt::MoveAction) || (action == Qt::CopyAction) ) ) {
        if ( data->hasFormat("application/file-list") ) {
            QByteArray  byteArray = data->data("application/file-list");
            QDataStream stream(&byteArray, QIODevice::ReadOnly);

            QList<QMediaContent> list;

            QString file;
            while ( !stream.atEnd() ) {
                stream >> file;
                list << QMediaContent( QUrl::fromLocalFile(file) );
            }

            if ( row < 0 ) {
                row = d->playlist->mediaCount();
            }

            beginResetModel();
            d->playlist->insertMedia(row, list);
            endResetModel();

            return true;
        } else if ( data->hasFormat("application/ids-list") ) {
            beginResetModel();

            QList<qint32> ids;

            QByteArray  byteArray = data->data("application/ids-list");
            QDataStream stream(&byteArray, QIODevice::ReadOnly);
            qint32      cid;

            while ( !stream.atEnd() ) {
                stream >> cid;
                ids << cid;
            }

            QList<QMediaContent> mediaList;

            int fix = 0;

            for ( qint32 id : ids ) {
                mediaList << d->playlist->media(id);
                if ( id < row ) {
                    fix++;
                }
            }

            for ( qint32 id : ids ) {
                d->playlist->removeMedia(id);
                fix++;
            }

            d->playlist->insertMedia(row - fix, mediaList);

            endResetModel();
        }
    }

    return QAbstractListModel::dropMimeData(data, action, row, column, parent);
}

Qt::DropActions PlaylistModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

Qt::DropActions PlaylistModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

bool PlaylistModel::removeRows(QList<int> rows)
{
    beginResetModel();

    int fix = 0;
    for ( int row : rows ) {
        d->playlist->removeMedia(row-fix);
        fix++;
    }

    endResetModel();

    return true;
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const
{
    if ( index.isValid() ) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemNeverHasChildren;
    }

    return Qt::ItemIsDropEnabled | Qt::ItemNeverHasChildren;
}
