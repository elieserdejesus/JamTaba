#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
#include "MainWindow.h"
#include "MainController.h"

namespace Ui {
class IODialog;
}

// This class is the base class for StandalonePreferencesDialog and VstPreferencesDialog

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    PreferencesDialog(Controller::MainController *mainController, MainWindow *mainWindow);
    virtual ~PreferencesDialog();

    void initialize();

    enum TAB {
        TAB_AUDIO, TAB_MIDI, TAB_VST, TAB_RECORDING
    };

protected slots:
    virtual void selectPreferencesTab(int index) = 0;

private slots:
    void selectRecordingPath();
    void setMultiTrackRecordingStatus(bool recordingActivated);

protected:
    Ui::IODialog *ui;
    Controller::MainController *mainController;
    MainWindow *mainWindow;

    // recording
    void populateRecordingTab();
    void selectRecordingTab();

    virtual void setupSignals();
    virtual void populateAllTabs();
};

#endif
