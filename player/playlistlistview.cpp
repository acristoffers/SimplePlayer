#include "player/playlistlistview.h"

#include <QDebug>

#include <QAction>
#include <QModelIndexList>
#include <QWidget>

#include "playlistmodel.h"

PlaylistListView::PlaylistListView(QWidget *parent) :
    QListView(parent)
{
    QAction *remove = new QAction(this);

    remove->setShortcut(QKeySequence::Delete);
    addAction(remove);
    connect( remove, SIGNAL( triggered() ), this, SLOT( removeSelected() ) );
}

void PlaylistListView::removeSelected()
{
    QModelIndexList indexes = selectedIndexes();
    QList<int>      ids;

    for ( QModelIndex index : indexes ) {
        ids << index.row();
    }

    qSort(ids);

    ( (PlaylistModel *) model() )->removeRows(ids);
}
