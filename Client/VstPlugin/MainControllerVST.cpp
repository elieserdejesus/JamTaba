#include "MainControllerVST.h"

#include "../midi/MidiDriver.h"

MainControllerVST::MainControllerVST(Persistence::Settings settings)
    :MainController(settings){
    qDebug() << "Creating MainControllerVST instance!";
}

Audio::AudioDriver* MainControllerVST::createAudioDriver(const Persistence::Settings &){
    return new Audio::NullAudioDriver();
}

Midi::MidiDriver* MainControllerVST::createMidiDriver(){
    return new Midi::NullMidiDriver();
}

void MainControllerVST::setCSS(QString css){
    qDebug() << "setting CSS";
    qApp->setStyleSheet(css);//qApp is a global variable created in dll main.
}


