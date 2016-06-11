#ifndef VST_PREFERENCES_DIALOG_H
#define VST_PREFERENCES_DIALOG_H

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

class VstPreferencesDialog : public PreferencesDialog
{
    Q_OBJECT

public:
    VstPreferencesDialog(QWidget *parent);
    void initialize(PreferencesTab initialTab, const Persistence::Settings *settings, const QMap<QString, QString> *jamRecorders) override;

protected slots:
    void selectTab(int index) override;

protected:
    void populateAllTabs() override;
};

#endif
