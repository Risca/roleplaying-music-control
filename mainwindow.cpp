#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "spotify.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Spotify *sp = new Spotify("Foo", "bar");
    connect(sp, SIGNAL(playlistsUpdated(QStringList)),
            this, SLOT(updatePlaylists(QStringList)));
    sp->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePlaylists(const QStringList &playlistNames)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(playlistNames);
}
