#include "sdlaudiodriverchangedialog.h"

#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include "sdlengine.h"

SDLAudioDriverChangeDialog::SDLAudioDriverChangeDialog(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f)
{
    originalDriver = SDLEngine::getCurrentAudioDriver();

    setWindowTitle(tr("Select audio driver"));

    driverList = new QComboBox(this);
    driverList->addItems(SDLEngine::getAudioDrivers());
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
    if (!SDLEngine::setAudioDriver(driverName)) {
        return;
    }

    deviceList->clear();
    deviceList->addItems(SDLEngine::getAudioDevices());
    adjustSize();
}
