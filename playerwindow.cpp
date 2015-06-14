#include "playerwindow.h"
#include "ui_playerwindow.h"

#include "spotify/spotify.h"

PlayerWindow::PlayerWindow(Spotify *spotifyContext, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerWindow),
    spotify(spotifyContext)
{
    ui->setupUi(this);
}

PlayerWindow::~PlayerWindow()
{
    delete spotify;
    delete ui;
}
