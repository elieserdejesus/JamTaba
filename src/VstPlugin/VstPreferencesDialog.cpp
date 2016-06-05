#include "VstPreferencesDialog.h"

VstPreferencesDialog::VstPreferencesDialog(QWidget *parent) :
    PreferencesDialog(parent)
{
    // in Vst plugin some preferences are not available
    // remove the first 3 tabs (audio, midi and VSTs)
    ui->prefsTab->removeTab(0);
    ui->prefsTab->removeTab(0);
    ui->prefsTab->removeTab(0);
}

void VstPreferencesDialog::initialize(PreferencesTab initialTab, const Persistence::Settings *settings, const QMap<QString, QString> *jamRecorders)
{
    PreferencesDialog::initialize(initialTab, settings, jamRecorders);
    int tabIndex = initialTab == PreferencesTab::TAB_RECORDING ? 0 : 1;
    ui->prefsTab->setCurrentIndex(tabIndex);
}

void VstPreferencesDialog::selectTab(int index)
{
    if (index == 0) //only the recording and metronome tabs are available in VST plugin
        populateRecordingTab();
    else
        populateMetronomeTab();
}

void VstPreferencesDialog::populateAllTabs(){
    populateRecordingTab();
    populateMetronomeTab();
}
