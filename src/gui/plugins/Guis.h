#ifndef DELAY_H
#define DELAY_H

#include <QWidget>

class QSlider;
class QLineEdit;

namespace Audio {
class JamtabaDelay;
class Plugin;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class PluginGui : public QWidget
{
    Q_OBJECT
public:
    explicit PluginGui(Audio::Plugin *plugin);

    QString getPluginName() const;
    Audio::Plugin *getPlugin() const;
protected:
    Audio::Plugin *plugin;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class DelayGui : public PluginGui
{
    Q_OBJECT

public:
    explicit DelayGui(Audio::JamtabaDelay *delayPlugin);
    ~DelayGui();
private slots:
    void on_sliderDelayReleased();
    void on_sliderFeedbackReleased();
    void on_sliderLevelReleased();

private:
    QSlider *sliderDelayTime;
    QLineEdit *lineEditDelayTime;

    QSlider *sliderFeedback;
    QLineEdit *lineEditFeedback;

    QSlider *sliderLevel;
    QLineEdit *lineEditLevel;
};

#endif // DELAY_H
