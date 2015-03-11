#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

#include "spotify.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    spotify = new Spotify("foo", "bar");
    connect(spotify, SIGNAL(playlistsUpdated(QStringList)),
            this, SLOT(updatePlaylists(QStringList)));
    connect(spotify, SIGNAL(songLoaded()), &retryTimer, SLOT(stop()));
    spotify->start();

    connect(&retryTimer, SIGNAL(timeout()), this, SLOT(playSong()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePlaylists(const QStringList &playlistNames)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(playlistNames);
    retryTimer.start(1000);
}

void MainWindow::playSong()
{
    spotify->playURI("spotify:track:0GLSeEhW4eUhckJ66SQ4GX");
}
