#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

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

private:
    Ui::LoginDialog *ui;
    Spotify *spotify;

private slots:
    void acceptLogin();
    void spotifyLoggedOut();
};

#endif // LOGINDIALOG_H
