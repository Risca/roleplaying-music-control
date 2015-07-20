#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QSettings>

class Spotify;

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    Spotify *takeOverSpotifyContext();
    bool isDungeonMaster() const;
    QString room() const;

signals:
    void tryLogin(const QString &username);
    void tryLogin(const QString &username, const QString &password);

private:
    Ui::LoginDialog *ui;
    Spotify *spotify;

    QSettings settings;
    bool useCredentialBlob;

private slots:
    void acceptLogin();
    void spotifyLoggedOut();
    void userChanged(const QString &user);
    void resetBlobUse();
};

#endif // LOGINDIALOG_H
