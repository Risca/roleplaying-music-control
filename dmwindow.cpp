#include "dmwindow.h"
#include "ui_dmwindow.h"

#include <QTimer>

#include "spotify/spotify.h"


DMWindow::DMWindow(Spotify *spotifyContext, const QString &room, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DMWindow),
    spotify(spotifyContext),
    topic(room),
    zmqContext(),
    zmqPublisher(zmqContext, ZMQ_PUB)
{
    ui->setupUi(this);

    zmqPublisher.bind("tcp://*:5556");

    connect(spotify, SIGNAL(playlistsUpdated(QStringList)),
            this, SLOT(updatePlaylists(QStringList)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
            spotify, SLOT(changePlaylist(int)));
    connect(ui->listWidget, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(playTrack(QModelIndex)));
    connect(spotify, SIGNAL(currentPlaylistUpdated(QList<SpotifyTrackInfo>)),
            this, SLOT(updateTracks(QList<SpotifyTrackInfo>)));
    connect(this, SIGNAL(trackClicked(QString)), spotify, SLOT(playURI(QString)));
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

    QString uri = item->data(Qt::UserRole).toString();
    emit trackClicked(uri);

    QByteArray zmqMsg = topic.toLocal8Bit() + " " + uri.toLocal8Bit();
    zmqPublisher.send(zmqMsg.begin(), zmqMsg.end());
}
