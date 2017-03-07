#include "PreferencesDialogPlugin.h"

PreferencesDialogPlugin::PreferencesDialogPlugin(QWidget *parent) :
    PreferencesDialog(parent)
{
    // in Vst plugin some preferences are not available
    // remove the first 3 tabs (audio, midi and VSTs)
    ui->prefsTab->removeTab(0);
    ui->prefsTab->removeTab(0);
    ui->prefsTab->removeTab(0);
}

void PreferencesDialogPlugin::initialize(PreferencesTab initialTab, const Persistence::Settings *settings, const QMap<QString, QString> &jamRecorders)
{
    PreferencesDialog::initialize(initialTab, settings, jamRecorders);
    int tabIndex = initialTab == PreferencesTab::TAB_RECORDING ? 0 : 1;
    ui->prefsTab->setCurrentIndex(tabIndex);
}

void PreferencesDialogPlugin::selectTab(int index)
{
    if (index == 0) //only the recording and metronome tabs are available in VST plugin
        populateMultiTrackRecordingTab();
    else
        populateMetronomeTab();
}

void PreferencesDialogPlugin::populateAllTabs(){
    populateMultiTrackRecordingTab();
    populateMetronomeTab();
}
