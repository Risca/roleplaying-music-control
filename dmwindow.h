#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QThread>

#include "3pp/cppzmq/zmq.hpp"
#include "spotify/spotifytrackinfo.h"

class QModelIndex;
class QStringList;
class Spotify;

namespace Ui {
class DMWindow;
}

class DMWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DMWindow(Spotify *spotifyContext, const QString &room, QWidget *parent = 0);
    ~DMWindow();

private:
    Ui::DMWindow *ui;
    Spotify *spotify;
    QThread audioThread;
    QSettings settings;
    QString topic;
    zmq::context_t zmqContext;
    zmq::socket_t zmqPublisher;

signals:
    void trackClicked(const QString &URI);

private slots:
    void updatePlaylists(const QStringList &playlistNames);
    void updateTracks(const QList<SpotifyTrackInfo> &trackList);
    void playTrack(const QModelIndex &index);
};

#endif // MAINWINDOW_H
