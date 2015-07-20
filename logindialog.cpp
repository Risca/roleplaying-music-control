#include "logindialog.h"
#include "ui_logindialog.h"

#include "spotify/spotify.h"

#include <QStringList>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    spotify(new Spotify),
    useCredentialBlob(false)
{
    ui->setupUi(this);
    QStringList usernames = settings.value("spotify/usernames").toStringList();
    if (!usernames.isEmpty()) {
        ui->usernameComboBox->addItems(usernames);
    }
    QString user = settings.value("spotify/last_user").toString();
    ui->usernameComboBox->setCurrentText(user);
    userChanged(user);
    connect(ui->usernameComboBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(userChanged(QString)));
    connect(ui->passwordLineEdit, SIGNAL(textEdited(QString)), this, SLOT(resetBlobUse()));

    bool dm = settings.value("login/dungeon_master").toBool();
    if (dm) {
        ui->dmCheckBox->setCheckState(Qt::Checked);
    }
    else {
        ui->dmCheckBox->setCheckState(Qt::Unchecked);
    }

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(acceptLogin()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(spotify, SIGNAL(loggedIn()), this, SLOT(accept()));
    connect(spotify, SIGNAL(loggedOut()), this, SLOT(spotifyLoggedOut()));
    connect(this, SIGNAL(tryLogin(QString)), spotify, SLOT(login(QString)));
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
    settings.setValue("login/dungeon_master", isDungeonMaster());
    delete spotify;
    delete ui;
}

Spotify *LoginDialog::takeOverSpotifyContext()
{
    Spotify *s = spotify;
    spotify = 0;
    return s;
}

bool LoginDialog::isDungeonMaster() const
{
    return ui->dmCheckBox->checkState() != Qt::Unchecked;
}

void LoginDialog::acceptLogin()
{
    QString username = ui->usernameComboBox->currentText();
    int idx = ui->usernameComboBox->findText(username);
    if (idx < 0) {
        ui->usernameComboBox->addItem(username);
    }
    if (useCredentialBlob) {
        emit tryLogin(username);
    }
    else {
        emit tryLogin(username, ui->passwordLineEdit->text());
    }
    this->setEnabled(false);
}

void LoginDialog::spotifyLoggedOut()
{
    this->setEnabled(true);
}

void LoginDialog::userChanged(const QString &user)
{
    const QMap<QString, QVariant> credentialBlobs = settings.value("spotify/credential_blobs").toMap();
    if (credentialBlobs.contains(user)) {
        ui->passwordLineEdit->setText("1234567890");
        useCredentialBlob = true;
    }
}

void LoginDialog::resetBlobUse()
{
    useCredentialBlob = false;
}
