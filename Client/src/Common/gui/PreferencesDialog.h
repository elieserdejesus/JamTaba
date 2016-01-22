#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
#include "MainWindow.h"

namespace Ui {
class IODialog;
}

// This class is the base class for StandalonePreferencesDialog and VstPreferencesDialog

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    PreferencesDialog(MainWindow *mainWindow);
    virtual ~PreferencesDialog();

    virtual void initialize(int initialTab, const Persistence::RecordingSettings &recordingSettings);

    enum TAB {
        TAB_AUDIO, TAB_MIDI, TAB_VST, TAB_RECORDING
    };

signals:
    void recordingPathSelected(const QString &newRecordingPath);
    void multiTrackRecordingStatusChanged(bool recording);

protected slots:
    virtual void selectTab(int index) = 0;

private slots:
    void selectRecordingPath();

protected:
    Ui::IODialog *ui;
    MainWindow *mainWindow;

    // recording
    void populateRecordingTab();
    void selectRecordingTab();

    virtual void setupSignals();
    virtual void populateAllTabs();

    Persistence::RecordingSettings recordingSettings;

};

#endif
