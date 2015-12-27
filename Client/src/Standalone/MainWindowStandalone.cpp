#include "MainWindowStandalone.h"
#include "StandAloneMainController.h"
#include "StandalonePreferencesDialog.h"
#include "NinjamRoomWindow.h"
#include <QTimer>

MainWindowStandalone::MainWindowStandalone(Controller::MainController *controller) :
    MainWindow(controller)
{
    initializePluginFinder();
    QTimer::singleShot(50, this, &MainWindowStandalone::restorePluginsList);
}

NinjamRoomWindow *MainWindowStandalone::createNinjamWindow(Login::RoomInfo roomInfo,
                                                           Controller::MainController *mainController)
{
    return new NinjamRoomWindow(this, roomInfo, mainController);
}

// implementing the MainWindow methods
bool MainWindow::canCreateSubchannels() const
{
    return true;
}

bool MainWindow::canUseFullScreen() const
{
    return true;
}

// ++++++++++++++++++++++++++++

void MainWindowStandalone::closeEvent(QCloseEvent *e)
{
    MainWindow::closeEvent(e);
    hide();// hide before stop main controller and disconnect from login server

    foreach (LocalTrackGroupView *trackGroup, localGroupChannels)
        trackGroup->closePluginsWindows();
}

void MainWindowStandalone::showEvent(QShowEvent *ent)
{
    Q_UNUSED(ent)
// TODO restorePluginsList(bool fromSettings)
    // wait 50 ms before restore the plugins list to avoid freeze the GUI in hidden state while plugins are loading
    // QTimer::singleShot(50, this, &MainWindowStandalone::restorePluginsList);
}

// ++++++++++++++++++++++
void MainWindowStandalone::showPreferencesDialog(int initialTab)
{
    stopCurrentRoomStream();

    Midi::MidiDriver *midiDriver = mainController->getMidiDriver();
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();
    if (audioDriver)
        audioDriver->stop();
    if (midiDriver)
        midiDriver->stop();

    StandalonePreferencesDialog dialog(mainController, this, initialTab);

    connect(&dialog,
            SIGNAL(ioPreferencesChanged(QList<bool>, int, int, int, int, int, int, int)),
            this,
            SLOT(setGlobalPreferences(QList<bool>, int, int, int, int, int, int, int)));

    int result = dialog.exec();
    if (result == QDialog::Rejected) {
        if (midiDriver)
            midiDriver->start();// restart audio and midi drivers if user cancel the preferences menu
        if (audioDriver)
            audioDriver->start();
    }
    /** audio driver parameters are changed in on_IOPropertiesChanged. This slot is always invoked when AudioIODialog is closed.*/
}

// ++++++++++++++++++++++

void MainWindowStandalone::initializePluginFinder()
{
    MainWindow::initializePluginFinder();

    Controller::StandaloneMainController *controller
        = dynamic_cast<Controller::StandaloneMainController *>(mainController);
    const Persistence::Settings settings = controller->getSettings();

    controller->initializePluginsList(settings.getVstPluginsPaths());// load the cached plugins. The cache can be empty.

    // checking for new plugins...
    if (controller->pluginsScanIsNeeded()) {// no vsts in database cache or new plugins detected in scan folders?
        if (settings.getVstScanFolders().isEmpty())
            controller->addDefaultPluginsScanPath();
        controller->saveLastUserSettings(getInputsSettings());// save config file before scan
        controller->scanPlugins(true);
    }
}

void MainWindowStandalone::handleServerConnectionError(QString msg)
{
    MainWindow::handleServerConnectionError(msg);
    (dynamic_cast<Controller::StandaloneMainController *>(mainController))->quit();
}

void MainWindowStandalone::setGlobalPreferences(QList<bool> midiInputsStatus, int audioDevice, int firstIn,
                                      int lastIn, int firstOut, int lastOut, int sampleRate,
                                      int bufferSize)
{
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();

#ifdef Q_OS_WIN
    audioDriver->setProperties(audioDevice, firstIn, lastIn, firstOut, lastOut, sampleRate,
                               bufferSize);
#endif
#ifdef Q_OS_MACX
    audioDriver->setProperties(sampleRate, bufferSize);
#endif
    mainController->storeIOSettings(firstIn, lastIn, firstOut, lastOut, audioDevice, sampleRate,
                                    bufferSize, midiInputsStatus);

    Midi::MidiDriver *midiDriver = mainController->getMidiDriver();
    midiDriver->setInputDevicesStatus(midiInputsStatus);

    mainController->updateInputTracksRange();

    foreach (LocalTrackGroupView *channel, localGroupChannels)
        channel->refreshInputSelectionNames();

    mainController->getMidiDriver()->start();
    try{
        mainController->getAudioDriver()->start();
    }
    catch (const std::runtime_error &error) {
        qCritical() << "Error starting audio device: " << QString::fromUtf8(error.what());
        QMessageBox::warning(this, "Audio error!",
                             "The audio device can't be started! Please check your audio device and try restart Jamtaba!");
        mainController->useNullAudioDriver();
    }
}
