#include "sdlaudiodriverchangedialog.h"

#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <SDL2/SDL.h>

SDLAudioDriverChangeDialog::SDLAudioDriverChangeDialog(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f)
{
    originalDriver = QString(SDL_GetCurrentAudioDriver());

    setWindowTitle(tr("Select audio driver"));

    QStringList drivers;
    for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
        drivers += QString(SDL_GetAudioDriver(i));
    }

    driverList = new QComboBox(this);
    driverList->addItems(drivers);
    deviceList = new QComboBox(this);
    connect(driverList, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(changeAudioDriver(QString)));
    driverList->setCurrentIndex(0);

    QDialogButtonBox *buttonBox =
            new QDialogButtonBox(QDialogButtonBox::Ok
                                 | QDialogButtonBox::Cancel,
                                 this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QLayout * layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Driver:")));
    layout->addWidget(driverList);
    layout->addWidget(new QLabel(tr("Devices:")));
    layout->addWidget(deviceList);
    layout->addWidget(buttonBox);
    setLayout(layout);
}

void SDLAudioDriverChangeDialog::accept()
{
    settings.setValue("SDL/AudioDriver", driverList->currentText());
    settings.setValue("SDL/AudioDevice", deviceList->currentText());
    QDialog::accept();
}

void SDLAudioDriverChangeDialog::reject()
{
    changeAudioDriver(originalDriver);
    QDialog::reject();
}

void SDLAudioDriverChangeDialog::changeAudioDriver(const QString &driverName)
{
    QString envString = "SDL_AUDIODRIVER=" + driverName;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    putenv(envString.toStdString().c_str());
    int rc = SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (rc != 0) {
        fprintf(stderr, "Failed to initialize \"%s\": %s\n",
                driverName.toStdString().c_str(),
                SDL_GetError());
        return;
    }

    QStringList devices;
    const int count = SDL_GetNumAudioDevices(0);
    for (int i = 0; i < count; ++i) {
        devices += QString(SDL_GetAudioDeviceName(i, 0));
    }
    deviceList->clear();
    deviceList->addItems(devices);
    adjustSize();
}
