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

signals:
    void tryLogin(const QString &username, const QString &password);

private:
    Ui::LoginDialog *ui;
    Spotify *spotify;

    QSettings settings;

private slots:
    void acceptLogin();
    void spotifyLoggedOut();
};

#endif // LOGINDIALOG_H
