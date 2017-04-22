
#ifndef __JamTaba__JamTabaAUPlugin__
#define __JamTaba__JamTabaAUPlugin__

#include "JamTaba.h"
#include "JamTabaPlugin.h"
#include <QString>
#include "MainControllerPlugin.h"
#include "MainWindowPlugin.h"
#include <QMacNativeWidget>

class JamTabaAUPlugin : public JamTabaAUInterface, public JamTabaPlugin
{

public:
    JamTabaAUPlugin(AudioUnit audioUnit);
  
    ~JamTabaAUPlugin();
    
    void initialize() override;
    
    void finalize();
    
    QString getHostName() override;
    
    float getSampleRate() const override;
    
    int getHostBpm() const override;
    
    bool hostIsPlaying() const override;
    
    qint32 getStartPositionForHostSync() const override;
    
    MainControllerPlugin *createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const override;
    
    // JamTabaAUInterface
    void processAudio(Float32 **inputs, Float32 **outputs, UInt16 inputsCount, UInt16 outputsCount, UInt32 framesToProcess) override;
    
    void resizeWindow(int newWidth, int newHeight);
    
    MainWindowPlugin *mainWindow;
    QMacNativeWidget *nativeView;
    
private:
    static QString CFStringToQString(CFStringRef str);

    AudioUnit audioUnit;
    AUHostState hostState;
    
    bool initializing;
    

};

#endif
