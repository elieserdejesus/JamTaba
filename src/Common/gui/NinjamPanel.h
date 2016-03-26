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

public:
    explicit NinjamPanel(QWidget *parent = 0);
    ~NinjamPanel();

    void createHostSyncButton(const QString &buttonText);
    void uncheckHostSyncButton();//used to uncheck the button when the sync with host fail (different BPMs)

    void setMuteButtonStatus(bool checked);
    void setPanSliderValue(int value);
    void setGainSliderValue(int value);

    void setBpiComboText(const QString &);
    void setBpmComboText(const QString &);

    int getPanSliderMaximumValue() const;
    int getGainSliderMaximumValue() const;
    int getCurrentBeatsPerAccent() const;

    void setBpi(int bpi);
    void setBpm(int bpm);
    void setCurrentBeat(int currentBeat);

    void setMetronomePeaks(float left, float right);

    int getIntervalShape() const;
    void setIntervalShape(int shape);

    void addMasterControls(QWidget *masterControlsPanel);

    void setFullViewStatus(bool fullView);

    void setLowContrastPaintInIntervalPanel(bool useLowContrastColors);

signals:
    void bpiComboActivated(const QString &);
    void bpmComboActivated(const QString &);
    void accentsComboChanged(int index);
    void gainSliderChanged(int value);
    void panSliderChanged(int value);
    void muteButtonClicked();
    void soloButtonClicked();
    void preferencesButtonClicked();
    void hostSyncStateChanged(bool syncWithHost);

protected:
    bool eventFilter(QObject *source, QEvent *ev);
    Ui::NinjamPanel *ui;
    QPushButton *hostSyncButton;// created only when running as vst plugin
private:
    void buildShapeModel();
    void buildAccentsdModel(int bpi);
    QStringList getBpiDividers(int bpi);
    static bool compareBpis(const QString &s1, const QString &s2);
    void selectClosestBeatsPerAccentInCombo(int currentBeatsPerAccent);
    void selectBeatsPerAccentInCombo(int beatsPerAccent);
private slots:
    void updateAccentsStatus(int index);
    void updateIntervalProgressShape(int index);
};

#endif // NINJAMPANEL_H
