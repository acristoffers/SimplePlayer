#include "player/playercontrols.h"
#include "ui_playercontrols.h"

#include <QIcon>
#include <QMediaPlaylist>
#include <QStyle>

struct PlayerControlsPrivate
{
    Ui::PlayerControls *ui;
    QMediaPlayer       *player;
};

PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent),
    d(new PlayerControlsPrivate)
{
    d->ui = new Ui::PlayerControls;
    d->ui->setupUi(this);

    d->ui->play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    d->ui->next->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    d->ui->prev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
}

PlayerControls::~PlayerControls()
{
}

void PlayerControls::setPlayer(QMediaPlayer *player)
{
    d->player = player;

    d->player->setNotifyInterval(1000);

    d->ui->volume->setMinimum(0);
    d->ui->volume->setMaximum(100);
    d->ui->volume->setValue(d->player->volume());

    connect(d->player,       SIGNAL(positionChanged(qint64)),           this,                  SLOT(updatePosition(qint64)));
    connect(d->player,       SIGNAL(stateChanged(QMediaPlayer::State)), this,                  SLOT(switchState(QMediaPlayer::State)));
    connect(d->player,       SIGNAL(volumeChanged(int)),                d->ui->volume,         SLOT(setValue(int)));

    connect(d->ui->play,     SIGNAL(clicked()),                         this,                  SLOT(switchPlayPause()));
    connect(d->ui->next,     SIGNAL(clicked()),                         d->player->playlist(), SLOT(next()));
    connect(d->ui->prev,     SIGNAL(clicked()),                         d->player->playlist(), SLOT(previous()));

    connect(d->ui->position, SIGNAL(valueChanged(int)),                 this,                  SLOT(setPosition(int)));

    connect(d->ui->volume,   SIGNAL(valueChanged(int)),                 d->player,             SLOT(setVolume(int)));
}

void PlayerControls::switchPlayPause()
{
    if (d->player->playlist()->isEmpty()) {
        d->player->pause();
        return;
    }

    if (d->player->state() == QMediaPlayer::PlayingState) {
        d->player->pause();
    } else {
        d->player->play();
    }
}

void PlayerControls::switchState(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState) {
        d->ui->play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        d->ui->play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void PlayerControls::setPosition(int pos)
{
    d->player->setPosition(pos);
}

void PlayerControls::updatePosition(qint64 pos)
{
    d->ui->position->setMinimum(0);
    d->ui->position->setMaximum(d->player->duration());
    d->ui->position->setValue(pos);
}
