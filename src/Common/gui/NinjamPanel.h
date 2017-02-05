#ifndef NINJAMPANEL_H
#define NINJAMPANEL_H

#include <QWidget>
#include <QStringList>
#include "intervalProgress/IntervalProgressWindow.h"

class QComboBox;
class QAbstractSlider;
class QPushButton;
class QObject;
class QEvent;
class TextEditorModifier;

namespace Ui {
class NinjamPanel;
}

class NinjamPanel : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamPanel(TextEditorModifier *bpiComboModifier, TextEditorModifier *bpmComboModifier, QWidget *parent = 0);
    ~NinjamPanel();

    void createHostSyncButton(const QString &buttonText);
    void uncheckHostSyncButton();//used to uncheck the button when the sync with host fail (different BPMs)
    bool hostSyncButtonIsChecked() const;

    void setMuteButtonStatus(bool checked);
    void setPanSliderValue(int value);
    void setGainSliderValue(int value);

    void setBpiComboText(const QString &);
    void setBpmComboText(const QString &);

    void setBpiComboPendingStatus(bool enabled);
    void setBpmComboPendingStatus(bool enabled);

    int getPanSliderMaximumValue() const;
    int getGainSliderMaximumValue() const;
    int getCurrentBeatsPerAccent() const;

    void setBpi(int bpi);
    void setBpm(int bpm);
    void setCurrentBeat(int currentBeat);

    void setMetronomePeaks(float left, float right, float rmsLeft, float rmsRight);

    int getIntervalShape() const;
    void setIntervalShape(int shape);

    void addMasterControls(QWidget *masterControlsPanel);

    void setLowContrastPaintInIntervalPanel(bool useLowContrastColors);

    void maximizeControlsWidget(bool maximize);

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
    void intervalShapeChanged(int newShape);

protected:
    bool eventFilter(QObject *source, QEvent *ev);
    void changeEvent(QEvent *) override;

    Ui::NinjamPanel *ui;
    QPushButton *hostSyncButton;// created only when running as vst plugin
    IntervalProgressWindow *metronomeFloatingWindow;

private:
    void buildShapeModel();
    void buildAccentsdModel(int bpi);
    QStringList getBpiDividers(int bpi);
    static bool compareBpis(const QString &s1, const QString &s2);
    void selectClosestBeatsPerAccentInCombo(int currentBeatsPerAccent);
    void selectBeatsPerAccentInCombo(int beatsPerAccent);
    void setupSignals();
    void translate();
    void initializeCombos(TextEditorModifier *bpiModifier, TextEditorModifier *bpmModifier);

private slots:
    void updateAccentsStatus(int index);
    void updateIntervalProgressShape(int index);
    void toggleMetronomeFloatingWindowVisibility(bool showFloatingWindow);
    void deleteFloatWindow();

    void handleBpiComboActication(const QString &);
    void handleBpmComboActication(const QString &);
};

#endif // NINJAMPANEL_H
