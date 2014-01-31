#include "player/musictreeitemmodel.h"

#include <QDebug>

#include <QBuffer>
#include <QByteArray>
#include <QImage>
#include <QMimeData>
#include <QPixmap>

#include <librarymanager.h>
#include <music.h>

/*
 * Data in QVariantList
 * put -1 or "" if not defined
 * 0 -> type       : Artist|Album|Music
 * 1 -> artist     : string
 * 2 -> album      : string
 * 3 -> title      : string
 * 4 -> track      : int
 * 5 -> year       : int
 * 6 -> file       : string
 */

#define DATASIZE 7

struct Item
{
    QVariantList  data;
    Item          *parent;
    QList<Item *> children;
    QPixmap       *_pixmap;

    QPixmap *pixmap()
    {
        if ( !_pixmap ) {
            QPixmap p = Music( data[6].toString() ).cover();
            if ( !p.isNull() ) {
                p = p.scaledToHeight(48);
            }
            _pixmap = new QPixmap(p);
        }

        return _pixmap;
    }

    int row() const
    {
        if ( parent && (parent->children.count() > 0) ) {
            return parent->children.indexOf( const_cast<Item *> (this) );
        }

        return 0;
    }

    Item()
    {
        _pixmap = nullptr;
    }

    ~Item()
    {
        if ( _pixmap ) {
            delete _pixmap;
        }
    }
};

struct MusicTreeItemModelPrivate
{
    Item          *root;
    QList<Item *> items;

    QVariantList makeData(QString type, QString artist, QString album = "", QString title = "", int track = -1, int year = -1, QString file = "")
    {
        QVariantList vl;

        vl.reserve(8);

        vl << type;
        vl << artist;
        vl << album;
        vl << title;
        vl << track;
        vl << year;
        vl << file;

        return vl;
    }

    void getData(QVariantList list, QString *type = nullptr, QString *artist = nullptr, QString *album = nullptr, QString *title = nullptr, int *track = nullptr, int *year = nullptr, QString *file = nullptr)
    {
        if ( list.size() != DATASIZE ) {
            return;
        }

        if ( type ) {
            *type = list[0].toString();
        }

        if ( artist ) {
            *artist = list[1].toString();
        }

        if ( album ) {
            *album = list[2].toString();
        }

        if ( title ) {
            *title = list[3].toString();
        }

        if ( track ) {
            *track = list[4].toInt();
        }

        if ( year ) {
            *year = list[5].toInt();
        }

        if ( file ) {
            *file = list[6].toString();
        }
    }
};

MusicTreeItemModel::MusicTreeItemModel(QObject *parent) :
    QAbstractItemModel(parent),
    d(new MusicTreeItemModelPrivate)
{
    d->root = new Item;
    reloadData();

    connect( LibraryManager::instance(), SIGNAL( scanFinished() ), this, SLOT( reloadData() ) );
}

MusicTreeItemModel::~MusicTreeItemModel()
{
    delete d->root;
    qDeleteAll(d->items);
    delete d;
}

int MusicTreeItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant MusicTreeItemModel::data(const QModelIndex &index, int role) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    if ( index.column() != 0 ) {
        return QVariant();
    }

    Item *item = static_cast<Item *> ( index.internalPointer() );

    QString type, artist, album, title;
    int     track;

    d->getData(item->data, &type, &artist, &album, &title, &track);

    if ( type == "Artist" ) {
        switch ( role ) {
            case Qt::DisplayRole:
            case Qt::ToolTipRole:
                return artist;

            case Qt::UserRole:
                return item->data;

            default:
                return QVariant();
        }
    } else if ( type == "Album" ) {
        switch ( role ) {
            case Qt::DisplayRole:
            case Qt::ToolTipRole:
                return album;

            case Qt::DecorationRole:
                return item->children.first()->pixmap()->copy();

            case Qt::UserRole:
                return item->data;

            default:
                return QVariant();
        }
    } else if ( type == "Music" ) {
        switch ( role ) {
            case Qt::DisplayRole:
            case Qt::ToolTipRole:
                return QString::number(track) + " - " + title;

            case Qt::UserRole:
                return item->data;

            default:
                return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags MusicTreeItemModel::flags(const QModelIndex &index) const
{
    if ( !index.isValid() ) {
        return 0;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
}

bool MusicTreeItemModel::hasChildren(const QModelIndex &parent) const
{
    if ( !parent.isValid() ) {
        return d->root->children.count() != 0;
    }

    Item *parentItem = static_cast<Item *> ( parent.internalPointer() );
    return parentItem->children.size() != 0;
}

QVariant MusicTreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    return QVariant();
}

QModelIndex MusicTreeItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if ( (column != 0) ) {
        return QModelIndex();
    }

    Item *parentItem;
    if ( !parent.isValid() ) {
        parentItem = d->root;
    } else {
        parentItem = static_cast<Item *> ( parent.internalPointer() );
    }

    if ( (parentItem->children.size() < row) || (parentItem->children.size() == 0) ) {
        return QModelIndex();
    }

    Item *child = parentItem->children.at(row);
    return createIndex(row, 0, child);
}

QMimeData *MusicTreeItemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData   *mimeData = new QMimeData;
    QByteArray  array;
    QDataStream stream(&array, QIODevice::WriteOnly);

    for ( QModelIndex index : indexes ) {
        if ( index.isValid() ) {
            Item *item = static_cast<Item *> ( index.internalPointer() );

            QString type, file;
            d->getData(item->data, &type);

            if ( type == "Artist" ) {
                for ( Item *album : item->children ) {
                    for ( Item *music : album->children ) {
                        d->getData(music->data, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &file);
                        stream << file;
                    }
                }
            } else if ( type == "Album" ) {
                for ( Item *music : item->children ) {
                    d->getData(music->data, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &file);
                    stream << file;
                }
            } else if ( type == "Music" ) {
                d->getData(item->data, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &file);
                stream << file;
            }
        }
    }

    mimeData->setData("application/file-list", array);
    return mimeData;
}

QStringList MusicTreeItemModel::mimeTypes() const
{
    QStringList l;

    l << "application/file-list";
    return l;
}

QModelIndex MusicTreeItemModel::parent(const QModelIndex &child) const
{
    if ( !child.isValid() ) {
        return QModelIndex();
    }

    Item *item = static_cast<Item *> ( child.internalPointer() );
    if ( item->parent == d->root ) {
        return QModelIndex();
    }

    return createIndex(item->parent->row(), 0, item->parent);
}

Qt::DropActions MusicTreeItemModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

int MusicTreeItemModel::rowCount(const QModelIndex &parent) const
{
    if ( !parent.isValid() ) {
        return d->root->children.count();
    }

    Item *item = static_cast<Item *> ( parent.internalPointer() );
    return item->children.size();
}

void MusicTreeItemModel::reloadData()
{
    beginResetModel();

    if ( d->items.count() > 0 ) {
        qDeleteAll(d->items);
    }

    d->items.clear();
    d->root->children.clear();

    QStringList artists = Music::artists();

    artists.sort();
    for ( QString artist : artists ) {
        Item *artistItem = new Item;

        artistItem->data   = d->makeData("Artist", artist);
        artistItem->parent = d->root;

        d->root->children.append(artistItem);
        d->items.append(artistItem);

        QStringList albums = Music::albumsForArtist(artist);
        albums.sort();

        for ( QString album : albums ) {
            Item *albumItem = new Item;

            albumItem->data   = d->makeData("Album", artist, album);
            albumItem->parent = artistItem;
            artistItem->children.append(albumItem);
            d->items.append(albumItem);

            QList<Music *> musics = Music::titlesForArtistAndAlbum(artist, album);
            for ( Music *m : musics ) {
                Item *musicItem = new Item;

                musicItem->data   = d->makeData( "Music", artist, album, m->title(), m->track(), m->year(), m->file() );
                musicItem->parent = albumItem;
                albumItem->children.append(musicItem);
                d->items.append(musicItem);
            }
        }
    }

    endResetModel();
}
