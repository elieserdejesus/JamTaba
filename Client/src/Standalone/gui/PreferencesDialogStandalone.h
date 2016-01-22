#ifndef STANDALONE_PREFERENCES_DIALOG_H
#define STANDALONE_PREFERENCES_DIALOG_H

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

namespace Audio {
class AudioDriver;
}

namespace Midi {
class MidiDriver;
}

class StandalonePreferencesDialog : public PreferencesDialog
{
    Q_OBJECT

public:
    StandalonePreferencesDialog(QWidget *parent, bool showAudioControlPanelButton, Audio::AudioDriver *audioDriver, Midi::MidiDriver *midiDriver);
    void initialize(int initialTab, const Persistence::Settings &settings) override;

public slots:
    void accept() override;

    void populateVstTab();

    void clearVstList();

signals:
    void ioPreferencesChanged(QList<bool> midiInputsStatus, int selectedAudioDevice, int firstIn,
                              int lastIn, int firstOut, int lastOut);

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

    Audio::AudioDriver *audioDriver;
    Midi::MidiDriver *midiDriver;

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
    void updateBlackBox();
    void clearScanFolderWidgets();

    void clearWidgetLayout(QWidget* widget);

    bool showAudioDriverControlPanelButton;

};

#endif
