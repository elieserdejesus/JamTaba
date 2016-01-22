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

void VstPreferencesDialog::selectTab(int index)
{
    Q_UNUSED(index)
    populateRecordingTab();//only the recording tab is available in VST plugin
}

void VstPreferencesDialog::populateAllTabs(){
    populateRecordingTab(); //only the recording tab is available in VST plugin
}
