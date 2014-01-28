#include "player/imagemodel.h"

#include <QDebug>

#include <librarymanager.h>
#include <image.h>

struct ImageModelPrivate
{
    QList<Image *> images;
};

ImageModel::ImageModel()
    : QAbstractItemModel(),
      d(new ImageModelPrivate)
{
    connect( LibraryManager::instance(), SIGNAL( scanFinished() ), this, SLOT( reloadData() ) );
    reloadData();
}

ImageModel::~ImageModel()
{
    qDeleteAll(d->images);
    delete d;
}

void ImageModel::reloadData()
{
    beginResetModel();

    qDeleteAll(d->images);
    d->images.clear();
    d->images = Image::allImages();

    endResetModel();
}

QModelIndex ImageModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if ( (column != 0) || ( (unsigned) row > LibraryManager::instance()->countImage() ) ) {
        return QModelIndex();
    }

    Image *image = d->images.at(row);
    return createIndex(row, column, image);
}

int ImageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return d->images.count();
}

bool ImageModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return false;
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && (role == Qt::DecorationRole) ) {
        Image *image = static_cast<Image *> ( index.internalPointer() );
        if ( image ) {
            return image->snapshot();
        }
    }

    return QVariant();
}

QModelIndex ImageModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

int ImageModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}
