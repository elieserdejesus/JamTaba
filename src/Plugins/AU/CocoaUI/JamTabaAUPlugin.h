
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
    JamTabaAUPlugin(AudioUnit audioUnit);
    
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
    
    void process(Float32 **inputs, Float32 **outputs, UInt16 inputsCount, UInt16 outputsCount, UInt32 framesToProcess, const AUHostState &hostState);
    
    void resizeWindow(int newWidth, int newHeight);
    
    JamTabaAudioUnitListener *listener;
    MainWindowPlugin *mainWindow;
    QMacNativeWidget *nativeView;
    
    static JamTabaAUPlugin *getInstance(AudioUnit unit);
    static void releaseInstance();
    

private:
    static QString CFStringToQString(CFStringRef str);
    
    static JamTabaAUPlugin *instance;
    AudioUnit audioUnit;
    AUHostState hostState;
    
    bool initializing;

};

class Listener : public JamTabaAudioUnitListener
{
public:
    Listener(JamTabaAUPlugin *auPlugin);
    
    void process(Float32 **inputs, Float32 **outputs, UInt16 inputsCount, UInt16 outputsCount, UInt32 framesToProcess, const AUHostState &hostState) override;
    
    void cleanUp() override;

private:
    JamTabaAUPlugin *auPlugin;
};

#endif
