#ifndef PLAYLISTLISTVIEW_H
#define PLAYLISTLISTVIEW_H

#include <QListView>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class PlaylistListView : public QListView
{
    Q_OBJECT
public:
    explicit PlaylistListView(QWidget *parent = 0);
protected slots:
    void removeSelected();
};
#endif // PLAYLISTLISTVIEW_H
