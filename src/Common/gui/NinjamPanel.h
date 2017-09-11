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

    void createHostSyncButton(const QString &hostName);
    void uncheckHostSyncButton(); // used to uncheck the button when the sync with host fail (different BPMs)
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

    int getAccentBeatsComboValue() const;

    void setAccentBeatsText(QString accentBeats);
    QString getAccentBeatsText() const;
    void setAccentBeatsTextEnabled(bool value);
    bool isAccentBeatsEnabled() const;

    void setBpi(int bpi);
    void setBpm(int bpm);
    void setCurrentBeat(int currentBeat);

    void setMetronomePeaks(float left, float right, float rmsLeft, float rmsRight);

    int getIntervalShape() const;
    void setIntervalShape(int shape);

    void addMasterControls(QWidget *masterControlsPanel);

    void setLowContrastPaintInIntervalPanel(bool useLowContrastColors);

    void maximizeControlsWidget(bool maximize);

    bool metronomeFloatingWindowIsVisible() const;

public slots:
    void updateStyleSheet();

signals:
    void bpiComboActivated(const QString &);
    void bpmComboActivated(const QString &);
    void accentsComboChanged(int index);
    void accentsTextChanged(const QString &);
    void gainSliderChanged(int value);
    void panSliderChanged(int value);
    void muteButtonClicked();
    void soloButtonClicked();
    void preferencesButtonClicked();
    void hostSyncStateChanged(bool syncWithHost);
    void intervalShapeChanged(int newShape);

public slots:
    void setMetronomeFloatingWindowVisibility(bool showFloatingWindow);

protected:
    void changeEvent(QEvent *) override;

    Ui::NinjamPanel *ui;
    QPushButton *hostSyncButton; // created only when running as vst plugin
    IntervalProgressWindow *metronomeFloatingWindow;

private:
    void buildShapeModel();
    void buildAccentsdModel(int bpi);
    QStringList getBpiDividers(int bpi);
    static bool compareBpis(const QString &s1, const QString &s2);
    void selectClosestBeatsPerAccentInCombo(int currentBeatsPerAccent);
    void selectBeatsPerAccentInCombo(int beatsPerAccent);
    void updateAccentsStatus();
    void setupSignals();
    void translate();
    void initializeCombos(TextEditorModifier *bpiModifier, TextEditorModifier *bpmModifier);

    QString hostName;

private slots:
    void handleAccentBeatsIndexChanged(int index);
    void handleAccentBeatsTextEdited();
    void updateIntervalProgressShape(int index);
    void deleteFloatWindow();

    void handleBpiComboActication(const QString &);
    void handleBpmComboActication(const QString &);
};

#endif // NINJAMPANEL_H
