
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
    

    JamTabaListener *listener;
    void callback(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess);

};

class Listener : public JamTabaListener
{
public:
    Listener(JamTabaAUPlugin *auPlugin)
    : auPlugin(auPlugin)
    {
        
    }
    
    void audioCallback(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess)
    {
        auPlugin->callback(inBuffer, outBuffer, inFramesToProcess);
    }
    
private:
    JamTabaAUPlugin *auPlugin;
};

#endif
