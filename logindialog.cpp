#include "logindialog.h"
#include "ui_logindialog.h"

#include "spotify/spotify.h"


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    spotify(NULL)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(acceptLogin()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

LoginDialog::~LoginDialog()
{
    delete spotify;
    delete ui;
}

Spotify *LoginDialog::takeOverSpotifyContext()
{
    Spotify *s = spotify;
    spotify = 0;
    return s;
}

void LoginDialog::acceptLogin()
{
    this->setEnabled(false);
    spotify = new Spotify(ui->usernameComboBox->currentText(),
                          ui->passwordLineEdit->text());
    connect(spotify, SIGNAL(loggedIn()), this, SLOT(accept()));
    connect(spotify, SIGNAL(loggedOut()), this, SLOT(spotifyLoggedOut()));
    spotify->start();
}

void LoginDialog::spotifyLoggedOut()
{
    delete spotify;
    spotify = NULL;
    this->setEnabled(true);
}
