#include "PreferencesDialog.h"

void PreferencesDialog::runPostInitialization(){
    //in mac some widgets are not visible
    ui->comboAudioDevice->setVisible(false);
    ui->asioDriverLabel->setVisible(false);
    ui->groupBoxInputs->setVisible(false);
    ui->groupBoxOutputs->setVisible(false);
}



