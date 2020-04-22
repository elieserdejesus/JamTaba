#ifndef STANDALONE_PREFERENCES_DIALOG_H
#define STANDALONE_PREFERENCES_DIALOG_H

#include "gui/PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

namespace audio {
class AudioDriver;
}

namespace midi {
class MidiDriver;
}

class PreferencesDialogStandalone : public PreferencesDialog
{
    Q_OBJECT

public:
    PreferencesDialogStandalone(QWidget *parent, bool showAudioControlPanelButton, audio::AudioDriver *audioDriver, midi::MidiDriver *midiDriver);
    void initialize(PreferencesTab initialTab, const persistence::Settings *settings, const QMap<QString, QString> &jamRecorders) override;

public slots:
    void accept() override;

    void populateVstTab();

    void clearVstList();

    void addFoundedVstPlugin(const QString &pluginName);
    void setCurrentScannedVstPlugin(const QString &pluginPath);

signals:
    void ioPreferencesChanged(QList<bool> midiInputsStatus, QString selectedInputAudioDevice, QString selectedOutputAudioDevice,
                              int firstIn, int lastIn, int firstOut, int lastOut);

    void sampleRateChanged(int newSampleRate);
    void bufferSizeChanged(int newBufferSize);

    void vstScanDirRemoved(const QString &scanDir);
    void vstScanDirAdded(const QString &newDir);

    void vstPluginAddedInBlackList(const QString &pluginPath);
    void vstPluginRemovedFromBlackList(const QString &pluginPath);

    void startingFullPluginsScan();
    void startingOnlyNewPluginsScan();
    void openingExternalAudioControlPanel(); // asio control panel in windows

private slots:
    void addBlackListedPlugins();
    void removeBlackListedPlugins();

    void showDialogToAddVstScanFolder();
    void removeSelectedVstScanFolder();

    void populateLastInputCombo();
    void populateLastOutputCombo();

    void changeAudioInputDevice(int index);
    void changeAudioOutputDevice(int index);

    void notifySampleRateChanged();
    void notifyBufferSizeChanged();

protected slots:
    void selectTab(int index) override;

protected:
    void setupSignals() override;
    void populateAllTabs() override;

private:

    audio::AudioDriver *audioDriver;
    midi::MidiDriver *midiDriver;

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

    void createWidgetsToNewFolder(const QString &path);

    void updateBlackBox();
    void clearScanFolderWidgets();

    void clearWidgetLayout(QWidget* widget);

    bool showAudioDriverControlPanelButton;

};

#endif
