#ifndef STANDALONE_LOCAL_TRACK_VIEW_H
#define STANDALONE_LOCAL_TRACK_VIEW_H

#include "gui/LocalTrackView.h"
#include "audio/core/AudioDriver.h"
#include "MainControllerStandalone.h"
#include "MidiToolsDialog.h"

#include <QMenu>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QPainterPath>

class LocalTrackViewStandalone : public LocalTrackView
{
    Q_OBJECT

    // custom properties defined in CSS files
    Q_PROPERTY(QColor midiRoutingArrowColor MEMBER midiRoutingArrowColor)

public:

    LocalTrackViewStandalone(controller::MainControllerStandalone* mainController, int channelID);

    void setPeakMetersOnlyMode(bool peakMetersOnly) override;
    void setActivatedStatus(bool unlighted) override;

    void updateGuiElements() override;

    inline FxPanel *getFxPanel()
    {
        return fxPanel;
    }

    qint32 getPluginFreeSlotIndex() const;

    void reset() override;

    void addPlugin(audio::Plugin *plugin, quint32 slotIndex, bool bypassed = false);

    QList<const audio::Plugin *> getInsertedPlugins() const;

    void refreshInputSelectionName();

    bool isMidi() const;
    bool isNoInput() const;

    void setToMidi();

    void setTintColor(const QColor &color) override;

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
    void showInputSelectionMenu(); // build and show the input selection menu
    void openMidiToolsDialog();
    void onMidiToolsDialogClosed();

    void setMidiLowerNote(const QString &lowerNote);
    void setMidiHigherNote(const QString &higherNote);
    void setTranspose(qint8 transposeValue);

    void toggleMidiNoteLearn(bool);

    void useLearnedMidiNote(quint8 midiNote);

private:
    controller::MainControllerStandalone* controller; //a 'casted' pointer just for convenience

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

    MidiActivityMeter *midiPeakMeter; // show midi activity

    void setMidiPeakMeterVisibility(bool visible);

    QString getInputChannelNameOnly(int inputIndex); // return the input channel name without the number/index

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
    QPixmap getInputTypePixmap();
    bool canUseMidiDeviceIndex(int midiDeviceIndex) const;

    void paintRoutingMidiArrow(int topMargin, int arrowSize, bool drawMidiWord);
    void paintReceivingRoutedMidiIndicator(int topMargin);

    void setAudioRelatedControlsStatus(bool enableControls);

    MidiToolsDialog *midiToolsDialog;

    QColor midiRoutingArrowColor;

    static QString getNoInputText();

};

#endif
