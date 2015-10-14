#ifndef MAINCONTROLLERVST_H
#define MAINCONTROLLERVST_H

#include "../MainController.h"

class MainControllerVST : public Controller::MainController{
public:
    MainControllerVST(Persistence::Settings settings);

    inline bool MainControllerVST::isRunningAsVstPlugin() const{
        return true;
    }

    Audio::AudioDriver* createAudioDriver(const Persistence::Settings &settings);

    void setCSS(QString css);

    inline Vst::PluginFinder* createPluginFinder(){return nullptr;}

    inline Audio::Plugin* createPluginInstance(const Audio::PluginDescriptor &descriptor){
        Q_UNUSED(descriptor);
        return nullptr;//vst plugin can't load other plugins
    }


    Midi::MidiDriver* createMidiDriver();

    inline void exit(){}

    inline void addDefaultPluginsScanPath(){}
    inline void scanPlugins(){}
};


#endif // MAINCONTROLLERVST_H
