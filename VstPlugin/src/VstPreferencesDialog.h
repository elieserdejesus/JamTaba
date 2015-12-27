#ifndef VST_PREFERENCES_DIALOG_H
#define VST_PREFERENCES_DIALOG_H

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include "MainWindow.h"

class VstPreferencesDialog : public PreferencesDialog
{
    Q_OBJECT

public:
    VstPreferencesDialog(Controller::MainController *mainController, MainWindow *mainWindow);

protected slots:
    void selectPreferencesTab(int index) override;

protected:
    void populateAllTabs() override;
};

#endif
