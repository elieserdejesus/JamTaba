#ifndef NINJAMPANEL_H
#define NINJAMPANEL_H

#include <QWidget>
#include <QStringList>
class QComboBox;
class QAbstractSlider;
class QPushButton;
class QObject;
class QEvent;

namespace Ui {
class NinjamPanel;
}

class NinjamPanel : public QWidget
{
    Q_OBJECT
signals:
    void bpiComboActivated(QString);
    void bpmComboActivated(QString);
    void accentsComboChanged(int index);
    void gainSliderChanged(int value);
    void panSliderChanged(int value);
    void muteButtonClicked();
    void soloButtonClicked();
    void hostSyncButtonClicked();
public:
    explicit NinjamPanel(QWidget *parent);
    ~NinjamPanel();

    void createHostSyncButton(QString buttonText);

    void setMuteButtonStatus(bool checked);
    void setPanSliderValue(int value);
    void setGainSliderValue(int value);

    void setHostSyncButtonAvailability(bool enabled);// availability? We need a better work here :)


    void setBpiComboText(QString);
    void setBpmComboText(QString);

    int getPanSliderMaximumValue() const;
    int getGainSliderMaximumValue() const;
    int getCurrentBeatsPerAccent() const;

    void setBpi(int bpi);
    void setBpm(int bpm);
    void setCurrentBeat(int currentBeat);

    void setMetronomePeaks(float left, float right);

    int getIntervalShape() const;
    void setIntervalShape(int shape);
protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *source, QEvent *ev);
    Ui::NinjamPanel *ui;
    QPushButton* hostSyncButton;//created only when running as vst plugin
private:
    void buildShapeModel();
    void buildAccentsdModel(int bpi);
    QStringList getBpiDividers(int bpi);
    static bool compareBpis(const QString& s1, const QString& s2);
    void selectClosestBeatsPerAccentInCombo(int currentBeatsPerAccent);
    void selectBeatsPerAccentInCombo(int beatsPerAccent);
private slots:
    void comboAccentsChanged(int index);
    void comboShapeChanged(int index);
};

#endif // NINJAMPANEL_H
