#ifndef DELAY_H
#define DELAY_H

#include <QWidget>

namespace Plugin {
    class JamtabaDelay;
}

namespace Audio {
class Plugin;
}

class PluginGui : public QWidget{
    Q_OBJECT
public:
    PluginGui(Audio::Plugin* plugin);

    QString getPluginName() const;//{return plugin->getName();}
protected:
    Audio::Plugin* plugin;
};

class DelayGui : public PluginGui
{
    Q_OBJECT

public:
    DelayGui( Plugin::JamtabaDelay* delayPlugin);
    ~DelayGui();

};

#endif // DELAY_H
