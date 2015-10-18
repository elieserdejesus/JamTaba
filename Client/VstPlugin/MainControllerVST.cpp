#include "MainControllerVST.h"

#include "../midi/MidiDriver.h"

class AudioDriverVST : public Audio::NullAudioDriver{
public:
    AudioDriverVST()
        :NullAudioDriver(){
        globalInputRange = Audio::ChannelRange(0, 4);//4 inputs
        globalOutputRange = Audio::ChannelRange(0,2);//2 outputs

    }
};

void MainControllerVST::setSampleRate(int newSampleRate){
    this->sampleRate = newSampleRate;
    MainController::setSampleRate(newSampleRate);
}

MainControllerVST::MainControllerVST(Persistence::Settings settings)
    :MainController(settings){

    qDebug() << "Creating MainControllerVST instance!";
}

Audio::AudioDriver* MainControllerVST::createAudioDriver(const Persistence::Settings &){
    return new AudioDriverVST();
}

Midi::MidiDriver* MainControllerVST::createMidiDriver(){
    return new Midi::NullMidiDriver();
}

void MainControllerVST::setCSS(QString css){
    qDebug() << "setting CSS";
    qApp->setStyleSheet(css);//qApp is a global variable created in dll main.
}


