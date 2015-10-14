#include "MainControllerVST.h"

MainControllerVST::MainControllerVST(Persistence::Settings settings)
    :MainController(settings){
    qDebug() << "Creating MainControllerVST instance!";
}

Audio::AudioDriver* MainControllerVST::buildAudioDriver(const Persistence::Settings &){
    return new Audio::NullAudioDriver();
}

void MainControllerVST::setCSS(QString css){
    qDebug() << "setting CSS";
    qApp->setStyleSheet(css);//qApp is a global variable created in dll main.
}


