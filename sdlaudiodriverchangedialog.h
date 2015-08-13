#ifndef SDLAUDIODRIVERCHANGEDIALOG_H
#define SDLAUDIODRIVERCHANGEDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QSettings>
#include <QString>

class QComboBox;

class SDLAudioDriverChangeDialog : public QDialog
{
    Q_OBJECT

public:
    SDLAudioDriverChangeDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);

    void accept();
    void reject();

public slots:
    void changeAudioDriver(const QString &driverName);

private:
    QComboBox *driverList;
    QComboBox *deviceList;
    QString originalDriver;
    QSettings settings;

};

#endif // SDLAUDIODRIVERCHANGEDIALOG_H
