#ifndef MAINCONTROLLERVST_H
#define MAINCONTROLLERVST_H

#include "MainControllerPlugin.h"

class JamTabaVSTPlugin;

class MainControllerVST : public MainControllerPlugin
{
public:
    MainControllerVST(const Persistence::Settings &settings, JamTabaVSTPlugin *plugin);
//    ~MainControllerVST();

    inline QString getJamtabaFlavor() const override
    {
        return "Vst Plugin";
    }

    void resizePluginEditor(int newWidth, int newHeight) override;

private:
    JamTabaVSTPlugin *vstPlugin;

};

#endif // MAINCONTROLLERVST_H
