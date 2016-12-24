
#ifndef __JamTaba__JamTabaAUPlugin__
#define __JamTaba__JamTabaAUPlugin__

#include "JamTaba.h"
#include "JamTabaPlugin.h"
#include <QString>
#include "MainControllerPlugin.h"
#include "MainWindowPlugin.h"
#include <QMacNativeWidget>


class Listener;


class JamTabaAUPlugin : public JamTabaPlugin
{

private:
    JamTabaAUPlugin();
    
public:
    ~JamTabaAUPlugin();
    
    void initialize() override;
    
    void finalize();
    
    QString getHostName() override;
    
    float getSampleRate() const override;
    
    int getHostBpm() const override;
    
    bool hostIsPlaying() const override;
    
    qint32 getStartPositionForHostSync() const override;
    
    MainControllerPlugin *createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const override;
    
    void process(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess);
    
    void resizeWindow(int newWidth, int newHeight);
    
    JamTabaAudioUnitListener *listener;
    MainWindowPlugin *mainWindow;
    QMacNativeWidget *nativeView;
    
    static JamTabaAUPlugin *getInstance();
    static void releaseInstance();
    

private:
    static QMacNativeWidget *createNativeView();
    static JamTabaAUPlugin *instance;

};

class Listener : public JamTabaAudioUnitListener
{
public:
    Listener(JamTabaAUPlugin *auPlugin);
    void process(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess) override;
    void cleanUp() override;

private:
    JamTabaAUPlugin *auPlugin;
};

#endif
