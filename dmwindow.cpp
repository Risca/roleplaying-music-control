#include "dmwindow.h"
#include "ui_dmwindow.h"

#include <QTimer>

#include "spotify/spotify.h"


DMWindow::DMWindow(Spotify *spotifyContext, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DMWindow),
    spotify(spotifyContext)
{
    ui->setupUi(this);

    connect(spotify, SIGNAL(playlistsUpdated(QStringList)),
            this, SLOT(updatePlaylists(QStringList)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
            spotify, SLOT(changePlaylist(int)));
    connect(ui->listWidget, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(playTrack(QModelIndex)));
    connect(spotify, SIGNAL(currentPlaylistUpdated(QList<SpotifyTrackInfo>)),
            this, SLOT(updateTracks(QList<SpotifyTrackInfo>)));
}

DMWindow::~DMWindow()
{
    delete spotify;
    delete ui;
}

void DMWindow::updatePlaylists(const QStringList &playlistNames)
{
    ui->comboBox->clear();
    ui->comboBox->addItems(playlistNames);
}

void DMWindow::updateTracks(const QList<SpotifyTrackInfo> &trackList)
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

void DMWindow::playTrack(const QModelIndex &index)
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
