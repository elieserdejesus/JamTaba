#include "maincontroller.h"
#include "audio/AudioDriverListener.h"
#include "audio/PortAudioDriver.h"
#include "audio/AudioMixer.h"
#include <QDebug>


//++++++++++++++++++++++++++++++
class Listener : public AudioDriverListenerAdapter{

private:
    MainController* mainController;
    AudioMixer* audioMixer;
    //AudioNode** oscillators;
    //AudioNode** oscillatorsGain;
    //AudioNode* output;
    //LocalInputAudioNode* localInputAudioNode;
    //static const int OSCS = 3;
public:
    Listener(MainController* controller){
        this->mainController = controller;
        this->audioMixer = new AudioMixer();
//        this->output = new MainOutputAudioNode();
//        this->localInputAudioNode = new LocalInputAudioNode();
//        this->oscillators = new AudioNode*[OSCS];
//        this->oscillatorsGain = new AudioNode*[OSCS];
//        for (int i = 0; i < OSCS; ++i) {
//            oscillators[i] = new OscillatorAudioNode(i * 200, ((AbstractAudioDriver*)controller->getAudioDriver())->getSampleRate());
//            oscillatorsGain[i] = new GainNode(0.1);
//            oscillators[i]->connect(oscillatorsGain[i]);
//            oscillatorsGain[i]->connect(output);
//        }
//        localInputAudioNode->connect(output);
    }

    ~Listener(){
        delete audioMixer;
//        for (int i = 0; i < OSCS; ++i) {
//            delete oscillators[i];
//            delete oscillatorsGain[i];
//        }
//        delete oscillators;
//        delete oscillatorsGain;
//        delete output;
//        delete localInputAudioNode;
    }

    virtual void driverStarted(){
        qDebug() << "MainControllre::driver started";
    }

    virtual void driverStopped(){
        qDebug() << "audio driver stopped";
    }

    virtual void processCallBack(AudioSamplesBuffer& in, AudioSamplesBuffer& out){
        audioMixer->process(in, out);
        //output->processReplacing(in, out);

        //out.add(in);

    }

    virtual void driverException(const char* msg){
        qDebug() << msg;
    }


};
//++++++++++++++++++++++++++++++

MainController::MainController()
{
    this->audioDriver = new PortAudioDriver();
    audioDriverListener = new Listener(this);
}

MainController::~MainController()
{
    if(this->audioDriver != NULL){
        this->audioDriver->stop();
        delete this->audioDriver;
        this->audioDriver = NULL;
    }
    if(this->audioDriverListener != NULL){
        delete this->audioDriverListener;
        this->audioDriverListener = NULL;
    }
}

void MainController::start()
{
    if(this->audioDriver != NULL){
        audioDriver->addListener(*audioDriverListener);
        audioDriver->start();
    }
}

void MainController::stop()
{
    if(this->audioDriver != NULL){
        this->audioDriver->release();
    }
}

AudioDriver *MainController::getAudioDriver() const
{
    return audioDriver;
}

