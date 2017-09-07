#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "persistence/Settings.h"
#include "recorder/JamRecorder.h"

namespace Ui {
class PreferencesDialog;
}

// This class is the base class for StandalonePreferencesDialog and VstPreferencesDialog

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    PreferencesDialog(QWidget *parent);
    virtual ~PreferencesDialog();

    enum PreferencesTab {
        TabAudio,
        TabMidi,
        TabVST,
        TabMultiTrackRecording,
        TabMetronome,
        TabLooper

    };

    virtual void initialize(PreferencesTab initialTab, const Persistence::Settings *settings, const QMap<QString, QString> &jamRecorders);

signals:
    void customMetronomeSelected(const QString &primaryBeatAudioFile, const QString &offBeatAudioFile, const QString &accentBeatAudioFile);
    void builtInMetronomeSelected(const QString &metronomeAlias);
    void multiTrackRecordingStatusChanged(bool recording);
    void jamRecorderStatusChanged(const QString &writerId, bool status);
    void recordingPathSelected(const QString &newRecordingPath);
    void encodingQualityChanged(float newEncodingQuality);
    void looperAudioEncodingFlagChanged(bool savingEncodedAudio);
    void looperWaveFilesBitDepthChanged(quint8 bitDepth);
    void looperFolderChanged(const QString &newLoopsFolder);

public slots:
    void accept() override;

protected slots:
    virtual void selectTab(int index) = 0;

private slots:
    void openRecordingPathBrowser();
    void openPrimaryBeatAudioFileBrowser();
    void openOffBeatAudioFileBrowser();
    void openAccentBeatAudioFileBrowser();

    void emitEncodingQualityChanged();

    void toggleCustomMetronomeSounds(bool usingCustomMetronome);
    void toggleBuiltInMetronomeSounds(bool usingBuiltInMetronome);

    void toggleRecording(bool recording);

private:
    void populateEncoderQualityComboBox();
    bool usingCustomEncodingQuality();
    QString selectAudioFile(QString caption, QString initialDir);
    void refreshMetronomeControlsStyleSheet();
    QString openAudioFileBrowser(const QString caption);
    QMap<QCheckBox *, QString> jamRecorderCheckBoxes;
    static QString getAudioFilesFilter();

protected:
    Ui::PreferencesDialog *ui;

    // recording
    void populateMultiTrackRecordingTab();
    void selectRecordingTab();

    // metronome
    void populateMetronomeTab();

    // looper
    void populateLooperTab();

    virtual void setupSignals();
    virtual void populateAllTabs();

    const Persistence::Settings *settings;
    QMap<QString, QString> jamRecorders;

};

#endif
