#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
#include "persistence/Settings.h"

namespace Ui {
class IODialog;
}

// This class is the base class for StandalonePreferencesDialog and VstPreferencesDialog

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    PreferencesDialog(QWidget *parent);
    virtual ~PreferencesDialog();

    virtual void initialize(int initialTab, const Persistence::Settings *settings);

    enum TAB {
        TAB_AUDIO, TAB_MIDI, TAB_VST, TAB_RECORDING, TAB_METRONOME
    };

signals:
    void recordingPathSelected(const QString &newRecordingPath);
    void customMetronomeSelected(const QString &primaryBeatAudioFile, const QString &secondaryBeatAudioFile);
    void builtInMetronomeSelected(const QString &metronomeAlias);
    void multiTrackRecordingStatusChanged(bool recording);

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
protected:
    Ui::IODialog *ui;

    // recording
    void populateRecordingTab();
    void selectRecordingTab();

    //metronome
    void populateMetronomeTab();

    virtual void setupSignals();
    virtual void populateAllTabs();

    const Persistence::Settings *settings;

};

#endif
