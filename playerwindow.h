#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QDialog>

class Spotify;

namespace Ui {
class PlayerWindow;
}

class PlayerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerWindow(Spotify *spotifyContext, QWidget *parent = 0);
    ~PlayerWindow();

private:
    Ui::PlayerWindow *ui;
    Spotify *spotify;
};

#endif // PLAYERWINDOW_H
