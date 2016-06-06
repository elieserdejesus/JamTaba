#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
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
        TAB_AUDIO, TAB_MIDI, TAB_VST, TAB_RECORDING, TAB_METRONOME
    };

    virtual void initialize(PreferencesTab initialTab, const Persistence::Settings *settings, const QMap<QString, QString> *jamRecorders);

signals:
    void customMetronomeSelected(const QString &primaryBeatAudioFile, const QString &secondaryBeatAudioFile);
    void builtInMetronomeSelected(const QString &metronomeAlias);
    void multiTrackRecordingStatusChanged(bool recording);
    void jamRecorderStatusChanged(const QString &writerId, bool status);
    void recordingPathSelected(const QString &newRecordingPath);

public slots:
    void accept() override;

protected slots:
    virtual void selectTab(int index) = 0;

private slots:
    void openRecordingPathBrowser();
    void openPrimaryBeatAudioFileBrowser();
    void openSecondaryBeatAudioFileBrowser();

    //void emitFirstBeatAudioFileChanged();
    //void emitSecondaryBeatAudioFileChanged();

    void toggleCustomMetronomeSounds(bool usingCustomMetronome);
    void toggleBuiltInMetronomeSounds(bool usingBuiltInMetronome);
private:
    QString selectAudioFile(QString caption, QString initialDir);
    void refreshMetronomeControlsStyleSheet();
    QString openAudioFileBrowser(const QString caption);
    QMap<QCheckBox *, QString> jamRecorderCheckBoxes;
protected:
    Ui::PreferencesDialog *ui;

    // recording
    void populateRecordingTab();
    void selectRecordingTab();

    //metronome
    void populateMetronomeTab();

    virtual void setupSignals();
    virtual void populateAllTabs();

    const Persistence::Settings *settings;
    const QMap<QString, QString> *jamRecorders;

};

#endif
