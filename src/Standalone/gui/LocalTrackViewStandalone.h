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

public:

    LocalTrackViewStandalone(Controller::MainControllerStandalone* mainController, int channelID);

    void setPeakMetersOnlyMode(bool peakMetersOnly, bool runningInMiniMode) override;
    void setUnlightStatus(bool unlighted) override;

    void updateGuiElements() override;

    inline FxPanel *getFxPanel()
    {
        return fxPanel;
    }

    void reset() override;

    void addPlugin(Audio::Plugin *PLUGIN, bool bypassed = false);

    QList<const Audio::Plugin *> getInsertedPlugins() const;

    void refreshInputSelectionName();

public slots:
    void setToNoInput();

protected slots:
    void setToMono(QAction *action) ;
    void setToStereo(QAction *action) ;
    void setToMidi(QAction *action) ;

protected:
    void translateUI() override;

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

    PeakMeter *midiPeakMeter;// show midi activity

    void setMidiPeakMeterVisibility(bool visible);

    QString getInputChannelNameOnly(int inputIndex);// return the input channel name without the number/index

    quint8 getMidiNoteNumber(const QString &midiNote) const;
    QString getMidiNoteText(quint8 midiNoteNumber) const;

    void startMidiNoteLearn();
    void stopMidiNoteLearn();

    MidiToolsDialog *midiToolsDialog;

    static const QString MIDI_ICON;
    static const QString MONO_ICON;
    static const QString STEREO_ICON;
    static const QString NO_INPUT_ICON;
};

#endif
