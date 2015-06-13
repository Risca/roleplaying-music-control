#include "logindialog.h"
#include "ui_logindialog.h"

#include "spotify/spotify.h"


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    spotify(new Spotify)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(acceptLogin()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(spotify, SIGNAL(loggedIn()), this, SLOT(accept()));
    connect(spotify, SIGNAL(loggedOut()), this, SLOT(spotifyLoggedOut()));
    connect(this, SIGNAL(tryLogin(QString,QString)), spotify, SLOT(login(QString,QString)));
    spotify->start();
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
    emit tryLogin(ui->usernameComboBox->currentText(),
                  ui->passwordLineEdit->text());
    this->setEnabled(false);
}

void LoginDialog::spotifyLoggedOut()
{
    this->setEnabled(true);
}
