#include "player/videomodel.h"

#include <video.h>

struct VideoModelPrivate
{
};

VideoModel::VideoModel()
    : QAbstractItemModel(),
      d(new VideoModelPrivate)
{
}

VideoModel::~VideoModel()
{
    delete d;
}

QModelIndex VideoModel::index(int row, int column, const QModelIndex &parent) const
{
}

int VideoModel::rowCount(const QModelIndex &parent) const
{
}

bool VideoModel::hasChildren(const QModelIndex &parent) const
{
}

QVariant VideoModel::data(const QModelIndex &index, int role) const
{
}
