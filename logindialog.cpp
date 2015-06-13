#include "logindialog.h"
#include "ui_logindialog.h"

#include "spotify/spotify.h"

#include <QStringList>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    spotify(new Spotify)
{
    ui->setupUi(this);
    QStringList usernames = settings.value("spotify/usernames").toStringList();
    if (!usernames.isEmpty()) {
        ui->usernameComboBox->addItems(usernames);
    }
    QString user = settings.value("spotify/last_user").toString();
    ui->usernameComboBox->setCurrentText(user);
    connect(ui->usernameComboBox, SIGNAL(currentIndexChanged(int)),
            ui->passwordLineEdit, SLOT(clear()));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(acceptLogin()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(spotify, SIGNAL(loggedIn()), this, SLOT(accept()));
    connect(spotify, SIGNAL(loggedOut()), this, SLOT(spotifyLoggedOut()));
    connect(this, SIGNAL(tryLogin(QString,QString)), spotify, SLOT(login(QString,QString)));
    spotify->start();
}

LoginDialog::~LoginDialog()
{
    QStringList usernames;
    for (int idx = 0; idx < ui->usernameComboBox->count(); ++idx) {
        usernames << ui->usernameComboBox->itemText(idx);
    }
    settings.setValue("spotify/usernames", usernames);
    settings.setValue("spotify/last_user", ui->usernameComboBox->currentText());
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
    QString username = ui->usernameComboBox->currentText();
    int idx = ui->usernameComboBox->findText(username);
    if (idx < 0) {
        ui->usernameComboBox->addItem(username);
    }
    emit tryLogin(username, ui->passwordLineEdit->text());
    this->setEnabled(false);
}

void LoginDialog::spotifyLoggedOut()
{
    this->setEnabled(true);
}
