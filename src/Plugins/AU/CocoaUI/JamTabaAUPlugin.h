
#ifndef __JamTaba__JamTabaAUPlugin__
#define __JamTaba__JamTabaAUPlugin__

#include "JamTaba.h"
#include "JamTabaPlugin.h"
#include <QString>
#include "MainControllerPlugin.h"


class Listener;


class JamTabaAUPlugin : public JamTabaPlugin
{
public:
    
    JamTabaAUPlugin();
    
    QString getHostName() override;
    
    float getSampleRate() const override;
    
    int getHostBpm() const override;
    
    bool hostIsPlaying() const override;
    
    qint32 getStartPositionForHostSync() const override;
    
    MainControllerPlugin *createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const override;
    

    JamTabaAudioUnitListener *listener;
    void process(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess);

};

class Listener : public JamTabaAudioUnitListener
{
public:
    Listener(JamTabaAUPlugin *auPlugin)
    : auPlugin(auPlugin)
    {
        
    }
    
    void process(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess) override
    {
        auPlugin->process(inBuffer, outBuffer, inFramesToProcess);
    }
    
    void cleanUp() override
    {
        //auPlugin->
        qDebug() << "CLEANING!!!";
    }
    
private:
    JamTabaAUPlugin *auPlugin;
};

#endif
