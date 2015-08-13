#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QDialog>

#include "zmqsubscriber.h"

class Spotify;

namespace Ui {
class PlayerWindow;
}

class PlayerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerWindow(Spotify *spotifyContext, const QString &room, QWidget *parent = 0);
    ~PlayerWindow();

signals:
    void playTrack(const QString &URI);

private:
    Ui::PlayerWindow *ui;
    Spotify *spotify;
    QString currentlyPlayingUri;
    ZmqSubscriber zmqSubscriber;

private slots:
    void handleZmqActivity(const QByteArray& data);
    void updateCurrentUri(const QString &uri);
    void changeAudioDriver();
};

#endif // PLAYERWINDOW_H
