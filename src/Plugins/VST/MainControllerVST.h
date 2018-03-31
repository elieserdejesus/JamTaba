#ifndef MAINCONTROLLERVST_H
#define MAINCONTROLLERVST_H

#include "MainControllerPlugin.h"

class JamTabaVSTPlugin;

class MainControllerVST : public MainControllerPlugin
{
public:
    MainControllerVST(const persistence::Settings &settings, JamTabaVSTPlugin *plugin);

    inline QString getJamtabaFlavor() const override
    {
        return "Vst Plugin";
    }

    void resizePluginEditor(int newWidth, int newHeight) override;

private:
    JamTabaVSTPlugin *vstPlugin;

};

#endif // MAINCONTROLLERVST_H
