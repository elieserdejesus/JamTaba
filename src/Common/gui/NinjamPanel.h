#ifndef NINJAMPANEL_H
#define NINJAMPANEL_H

#include <QFrame>
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

class NinjamPanel : public QFrame
{
    Q_OBJECT

public:
    explicit NinjamPanel(TextEditorModifier *bpiComboModifier, TextEditorModifier *bpmComboModifier, TextEditorModifier *accentBeatsModifier, QWidget *parent);
    ~NinjamPanel();

    void createHostSyncButton(const QString &hostName);
    void uncheckHostSyncButton(); // used to uncheck the button when the sync with host fail (different BPMs)
    bool hostSyncButtonIsChecked() const;

    void hideMidiSyncCheckBox();  // used to show the sync checkbox in standalone only

    void setBpiComboText(const QString &);
    void setBpmComboText(const QString &);

    void setBpiComboPendingStatus(bool enabled);
    void setBpmComboPendingStatus(bool enabled);

    int getAccentBeatsComboValue() const;

    void setAccentBeatsText(QString accentBeats);
    QString getAccentBeatsText() const;
    QList<int> getAccentBeats() const;

    void setAccentBeatsReadOnly(bool value);
    void setAccentBeatsVisible(bool value);

    bool isShowingAccents() const;

    void setBpi(int bpi);
    void setBpm(int bpm);
    void setCurrentBeat(int currentBeat);

    int getBpi() const;
    int getBpm() const;

    int getIntervalShape() const;
    void setIntervalShape(int shape);

    void setLowContrastPaintInIntervalPanel(bool useLowContrastColors);

    void setCollapseMode(bool collapsed);

    void maximizeControlsWidget(bool maximize);

    void setMetronomeFloatingWindow(IntervalProgressWindow *floatingWindow);

signals:
    void bpiComboActivated(const QString &);
    void bpmComboActivated(const QString &);
    void accentsComboChanged(int index);
    void accentsBeatsChanged(const QList<int> &);
    void hostSyncStateChanged(bool syncWithHost);
    void intervalShapeChanged(int newShape);
    void midiSyncChanged(bool syncOn);

protected:
    void changeEvent(QEvent *) override;

    Ui::NinjamPanel *ui;
    QPushButton *hostSyncButton; // created only when running as vst plugin


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
    void initializeCombos(TextEditorModifier *bpiModifier, TextEditorModifier *bpmModifier, TextEditorModifier *accentBeatsModifier);

    QString hostName;

    IntervalProgressWindow *metronomeFloatingWindow;

private slots:
    void handleAccentBeatsIndexChanged(int index);
    void handleAccentBeatsTextEdited();
    void updateIntervalProgressShape(int index);

    void handleBpiComboActication(const QString &);
    void handleBpmComboActication(const QString &);
};

#endif // NINJAMPANEL_H
