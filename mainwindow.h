#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QTimer>
#include <QThread>

#include "spotify/spotifytrackinfo.h"

class QModelIndex;
class QStringList;
class Spotify;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Spotify *spotify;
    QThread audioThread;

private slots:
    void updatePlaylists(const QStringList &playlistNames);
    void updateTracks(const QList<SpotifyTrackInfo> &trackList);
    void playTrack(const QModelIndex &index);
};

#endif // MAINWINDOW_H
