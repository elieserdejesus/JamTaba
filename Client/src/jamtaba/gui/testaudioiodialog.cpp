#ifndef TESTE_AUDIO_IO_DIALOG
#define TESTE_AUDIO_IO_DIALOG


#include "audioiodialog.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include "../audio/PortAudioDriver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PortAudioDriver driver;
    AudioIODialog dialog(driver);

    dialog.show();
    return a.exec();
}

#endif
