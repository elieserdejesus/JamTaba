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

    virtual void initialize(int initialTab, const Persistence::Settings &settings);

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

    // recording
    void populateRecordingTab();
    void selectRecordingTab();

    virtual void setupSignals();
    virtual void populateAllTabs();

    Persistence::Settings settings;

};

#endif
