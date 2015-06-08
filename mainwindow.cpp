#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

#include "spotify/spotify.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    spotify(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete spotify;
    delete ui;
}

void MainWindow::setSpotifyContext(Spotify *s)
{
    if (s == spotify) {
        return;
    }

    if (spotify) {
        spotify->deleteLater();
    }
    spotify = s;

    connect(spotify, SIGNAL(playlistsUpdated(QStringList)),
            this, SLOT(updatePlaylists(QStringList)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
            spotify, SLOT(changePlaylist(int)));
    connect(ui->listWidget, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(playTrack(QModelIndex)));
    connect(spotify, SIGNAL(currentPlaylistUpdated(QList<SpotifyTrackInfo>)),
            this, SLOT(updateTracks(QList<SpotifyTrackInfo>)));
}

void MainWindow::updatePlaylists(const QStringList &playlistNames)
{
    ui->comboBox->clear();
    ui->comboBox->addItems(playlistNames);
}

void MainWindow::updateTracks(const QList<SpotifyTrackInfo> &trackList)
{
    ui->listWidget->clear();
    QList<SpotifyTrackInfo>::const_iterator it = trackList.constBegin();
    while (it != trackList.constEnd()) {
        QListWidgetItem *item = new QListWidgetItem(it->name);
        item->setData(Qt::UserRole, it->URI);
        ui->listWidget->addItem(item);
        ++it;
    }
}

void MainWindow::playTrack(const QModelIndex &index)
{
    QListWidgetItem *item = ui->listWidget->item(index.row());
    if (!item) {
        return;
    }

    QVariant uri = item->data(Qt::UserRole);
    if (!uri.canConvert<QString>()) {
        return;
    }

    spotify->playURI(uri.toString());
}
