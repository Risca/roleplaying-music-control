#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>

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
    void updateTracks(const QStringList &trackList);
    void playSong();
};

#endif // MAINWINDOW_H
