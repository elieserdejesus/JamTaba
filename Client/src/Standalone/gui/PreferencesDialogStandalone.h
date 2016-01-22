#ifndef STANDALONE_PREFERENCES_DIALOG_H
#define STANDALONE_PREFERENCES_DIALOG_H

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include "MainWindow.h"
#include "MainControllerStandalone.h"

class StandalonePreferencesDialog : public PreferencesDialog
{
    Q_OBJECT

public:
    StandalonePreferencesDialog(Controller::MainControllerStandalone *mainController, MainWindow *mainWindow);
    void initialize(int initialTab, const Persistence::RecordingSettings &recordingSettings) override;

public slots:
    void accept() override;

signals:
    void ioPreferencesChanged(QList<bool> midiInputsStatus, int selectedAudioDevice, int firstIn,
                              int lastIn, int firstOut, int lastOut, int sampleRate,
                              int bufferSize);

private slots:
    void addBlackListedPlugins();
    void removeBlackListedPlugins();

    void scansFully();
    void scanNewPlugins();

    void openExternalAudioControlPanel();// asio control panel in windows

    void addVstScanFolder();
    void removeVstscanFolder();

    void populateLastInputCombo();
    void populateLastOutputCombo();

    void changeAudioDevice(int index);

    void populateVstTab();

protected slots:
    void selectTab(int index) override;

protected:
    void setupSignals() override;
    void populateAllTabs() override;

private:

    Controller::MainControllerStandalone* controller;

    void selectAudioTab();
    void selectMidiTab();
    void selectVstPluginsTab();

    void populateAsioDriverCombo();
    void populateFirstInputCombo();
    void populateFirstOutputCombo();

    void populateSampleRateCombo();
    void populateBufferSizeCombo();
    void populateAudioTab();

    void populateMidiTab();

    void addVstFolderToScan(QString folder);
    void createWidgetsToNewFolder(QString path);
    void updateVstList(QString path);
    void updateBlackBox(QString path, bool add);
    void clearScanFolderWidgets();

    void clearWidgetLayout(QWidget* widget);

};

#endif
