#ifndef STANDALONE_PREFERENCES_DIALOG_H
#define STANDALONE_PREFERENCES_DIALOG_H

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include "MainControllerStandalone.h"

class StandalonePreferencesDialog : public PreferencesDialog
{
    Q_OBJECT

public:
    StandalonePreferencesDialog(Controller::MainControllerStandalone *mainController, QWidget *parent);
    void initialize(int initialTab, const Persistence::Settings &settings) override;

public slots:
    void accept() override;

    void populateVstTab();

signals:
    void ioPreferencesChanged(QList<bool> midiInputsStatus, int selectedAudioDevice, int firstIn,
                              int lastIn, int firstOut, int lastOut);

    void sampleRateChanged(int newSampleRate);
    void bufferSizeChanged(int newBufferSize);

    void vstScanDirRemoved(const QString &scanDir);
    void vstScanDirAdded(const QString &newDir);

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

    void notifySampleRateChanged();
    void notifyBufferSizeChanged();

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

    void createWidgetsToNewFolder(QString path);
    void updateVstList(QString path);
    void updateBlackBox(QString path, bool add);
    void clearScanFolderWidgets();

    void clearWidgetLayout(QWidget* widget);

};

#endif
