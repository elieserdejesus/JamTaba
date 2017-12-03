#ifndef PREFERENCES_DIALOG_PLUGIN_H
#define PREFERENCES_DIALOG_PLUGIN_H

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

class PreferencesDialogPlugin : public PreferencesDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialogPlugin(QWidget *parent);
    void initialize(PreferencesTab initialTab, const Persistence::Settings *settings, const QMap<QString, QString> &jamRecorders) override;

protected slots:
    void selectTab(int index) override;

protected:
    void populateAllTabs() override;
};

#endif
