#ifndef METRONOMEPANEL_H
#define METRONOMEPANEL_H

#include <QWidget>

namespace Ui {
class MetronomePanel;
}

class MetronomePanel : public QWidget
{
    Q_OBJECT

public:
    explicit MetronomePanel(QWidget *parent);
    ~MetronomePanel();

    int getPanSliderMaximumValue() const;
    int getGainSliderMaximumValue() const;

    void setMuteButtonStatus(bool checked);
    void setPanSliderValue(int value);
    void setGainSliderValue(int value);
    void setFloatingWindowButtonChecked(bool checked);

    void setMetronomePeaks(float left, float right, float rmsLeft, float rmsRight);

    void setTintColor(const QColor &color);

signals:
    void gainSliderChanged(int value);
    void panSliderChanged(int value);
    void muteButtonClicked();
    void soloButtonClicked();
    void preferencesButtonClicked();
    void floatingWindowButtonToggled(bool checked);

public slots:
    void updateStyleSheet();

private:
    Ui::MetronomePanel *ui;

    void setupSignals();
};

#endif // METRONOMEPANEL_H
