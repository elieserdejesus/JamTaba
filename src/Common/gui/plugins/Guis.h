#ifndef DELAY_H
#define DELAY_H

#include <QWidget>

class QSlider;
class QLineEdit;

namespace audio {
class JamtabaDelay;
class Plugin;
}


class PluginGui : public QWidget
{
    Q_OBJECT

public:
    explicit PluginGui(audio::Plugin *plugin);

    QString getPluginName() const;
    audio::Plugin *getPlugin() const;

protected:
    audio::Plugin *plugin;

};


class DelayGui : public PluginGui
{
    Q_OBJECT

public:
    explicit DelayGui(audio::JamtabaDelay *delayPlugin);
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
