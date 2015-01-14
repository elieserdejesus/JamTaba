#include "maincontroller.h"
#include "audio/AudioDriverListener.h"
#include "audio/PortAudioDriver.h"
#include <QDebug>


//++++++++++++++++++++++++++++++
class Listener : public AudioDriverListenerAdapter{
    virtual void driverStarted(){
        qDebug() << "driver started";
    }

    virtual void driverStopped(){
        qDebug() << "audio driver stopped";
    }

    virtual void processCallBack(float** in, float**out, const int samplesToProcess){
        //qDebug() << "procesCallBack << samples: " + samplesToProcess;
        for(int i=0; i < samplesToProcess; i++){
            out[0][i] = in[0][i];
            out[1][i] = in[0][i];
        }
    }

    virtual void driverException(const char* msg){
        qDebug() << msg;
    }


};
//++++++++++++++++++++++++++++++

MainController::MainController()
{
    this->audioDriver = new PortAudioDriver();
    audioDriverListener = new Listener();
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

