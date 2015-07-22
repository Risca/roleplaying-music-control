#include "playerwindow.h"
#include "ui_playerwindow.h"

#include "spotify/spotify.h"

PlayerWindow::PlayerWindow(Spotify *spotifyContext, const QString &room, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerWindow),
    spotify(spotifyContext),
    zmqSubscriber("tcp://risca.servebeer.com:5558", room)
{
    ui->setupUi(this);

    connect(&zmqSubscriber, SIGNAL(dataReceived(QByteArray)), this, SLOT(handleZmqActivity(QByteArray)));
    connect(spotify, SIGNAL(songLoaded(QString)), this, SLOT(updateCurrentUri(QString)));
    connect(this, SIGNAL(playTrack(QString)), spotify, SLOT(playURI(QString)));

    zmqSubscriber.start();
}

PlayerWindow::~PlayerWindow()
{
    delete spotify;
    delete ui;
}

void PlayerWindow::handleZmqActivity(const QByteArray &data)
{
    int idx = data.indexOf(QChar::Space);
    if (idx < 0) {
        return;
    }

    QString uri = data.mid(idx + 1);
    if (uri != currentlyPlayingUri) {
        emit playTrack(uri);
    }
}

void PlayerWindow::updateCurrentUri(const QString &uri)
{
    currentlyPlayingUri = uri;
    ui->songLabel->setText(Spotify::songNameFromUri(currentlyPlayingUri));
}
