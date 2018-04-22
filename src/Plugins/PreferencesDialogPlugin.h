#ifndef PREFERENCES_DIALOG_PLUGIN_H
#define PREFERENCES_DIALOG_PLUGIN_H

#include "gui/PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

using persistence::Settings;

class PreferencesDialogPlugin : public PreferencesDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialogPlugin(QWidget *parent);
    void initialize(PreferencesTab initialTab, const Settings *settings, const QMap<QString, QString> &jamRecorders) override;

protected slots:
    void selectTab(int index) override;

protected:
    void populateAllTabs() override;
};

#endif
