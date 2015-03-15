#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

#include "spotify/spotify.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    spotify = new Spotify("foo", "bar");
    connect(spotify, SIGNAL(playlistsUpdated(QStringList)),
            this, SLOT(updatePlaylists(QStringList)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
            spotify, SLOT(changePlaylist(int)));
    connect(spotify, SIGNAL(currentPlaylistUpdated(QStringList)),
            this, SLOT(updateTracks(QStringList)));
    connect(spotify, SIGNAL(loggedIn()), this, SLOT(playSong()));
    spotify->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePlaylists(const QStringList &playlistNames)
{
    ui->comboBox->clear();
    ui->comboBox->addItems(playlistNames);
}

void MainWindow::updateTracks(const QStringList &trackList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(trackList);
}

void MainWindow::playSong()
{
    spotify->playURI("spotify:track:0GLSeEhW4eUhckJ66SQ4GX");
}
