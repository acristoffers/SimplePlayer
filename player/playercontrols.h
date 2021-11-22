#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QMediaContent>
#include <QMediaPlayer>
#include <QWidget>

struct PlayerControlsPrivate;

class PlayerControls : public QWidget
{
    Q_OBJECT
public:
    PlayerControls(QWidget*);
    ~PlayerControls();

    void setPlayer(QMediaPlayer *player);
private:
    PlayerControlsPrivate *d;
public slots:
    void switchPlayPause();
    void switchState(QMediaPlayer::State);
    void setPosition(int);

    void updatePosition(qint64);
};
#endif // PLAYERCONTROLS_H
