#ifndef STANDALONE_LOCAL_TRACK_VIEW_H
#define STANDALONE_LOCAL_TRACK_VIEW_H

#include "LocalTrackView.h"
#include "audio/core/AudioDriver.h"
#include "MainControllerStandalone.h"
#include "MidiToolsDialog.h"

#include <QMenu>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>

class LocalTrackViewStandalone : public LocalTrackView
{
    Q_OBJECT

    // custom properties defined in CSS files
    Q_PROPERTY(QColor midiRoutingArrowColor MEMBER midiRoutingArrowColor)

public:

    LocalTrackViewStandalone(Controller::MainControllerStandalone* mainController, int channelID);

    void setPeakMetersOnlyMode(bool peakMetersOnly) override;
    void setActivatedStatus(bool unlighted) override;

    void updateGuiElements() override;

    inline FxPanel *getFxPanel()
    {
        return fxPanel;
    }

    qint32 getPluginFreeSlotIndex() const;

    void reset() override;

    void addPlugin(Audio::Plugin *plugin, quint32 slotIndex, bool bypassed = false);

    QList<const Audio::Plugin *> getInsertedPlugins() const;

    void refreshInputSelectionName();

signals:
    void trackInputChanged();

public slots:
    void setToNoInput();
    void setMidiRouting(bool routingMidiToFirstSubchannel);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void translateUI() override;
    bool eventFilter(QObject *target, QEvent *event) override;
    void setupMetersLayout() override;

protected slots:
    void setToMono(QAction *action) ;
    void setToStereo(QAction *action) ;
    void setToMidi(QAction *action) ;

private slots:
    void showInputSelectionMenu();// build and show the input selection menu
    void openMidiToolsDialog();
    void onMidiToolsDialogClosed();

    void setMidiLowerNote(const QString &lowerNote);
    void setMidiHigherNote(const QString &higherNote);
    void setTranspose(qint8 transposeValue);

    void toggleMidiNoteLearn(bool);

    void useLearnedMidiNote(quint8 midiNote);

private:
    Controller::MainControllerStandalone* controller;//a 'casted' pointer just for convenience

    QMenu *createMonoInputsMenu(QMenu *parentMenu);
    QMenu *createStereoInputsMenu(QMenu *parentMenu);
    QMenu *createMidiInputsMenu(QMenu *parentMenu);
    QPushButton *createMidiToolsButton();

    QWidget *createInputPanel();
    QPushButton *createInputSelectionButton(QWidget *parent);
    QLabel *createInputTypeIconLabel(QWidget *parent);
    FxPanel *createFxPanel();

    QPushButton *midiToolsButton;
    QPushButton *inputSelectionButton;
    QLabel *inputTypeIconLabel;
    QWidget *inputPanel;
    FxPanel *fxPanel;

    MidiActivityMeter *midiPeakMeter;// show midi activity

    void setMidiPeakMeterVisibility(bool visible);

    QString getInputChannelNameOnly(int inputIndex);// return the input channel name without the number/index

    quint8 getMidiNoteNumber(const QString &midiNote) const;
    QString getMidiNoteText(quint8 midiNoteNumber) const;

    void startMidiNoteLearn();
    void stopMidiNoteLearn();

    bool canShowMidiToolsButton() const;
    bool canShowInputTypeIcon() const;

    bool isFirstSubchannel() const;

    void updateInputText();
    void updateInputIcon();

    QString getInputText();
    QString getAudioInputText();
    QString getMidiInputText();
    QString getInputTypeIconFile();
    bool canUseMidiDeviceIndex(int midiDeviceIndex) const;

    void paintRoutingMidiArrow(const QColor &color, int topMargin, int arrowSize, bool drawSolidLine, bool drawMidiWord);
    void paintReceivingRoutedMidiIndicator(const QColor &color, int topMargin, int arrowSize);

    void setAudioRelatedControlsStatus(bool enableControls);

    MidiToolsDialog *midiToolsDialog;

    QColor midiRoutingArrowColor;

    static const QString MIDI_ICON;
    static const QString MONO_ICON;
    static const QString STEREO_ICON;
    static const QString NO_INPUT_ICON;

    static QString getNoInputText();

};

#endif
