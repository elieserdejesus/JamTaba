#pragma once

#include "BaseTrackView.h"
#include <QWidget>
#include <QTimer>
#include "PeakMeter.h"

class QMenu;
class FxPanel;
class QFrame;
class QPushButton;
class QLabel;
class QSpacerItem;

namespace Audio {
class Plugin;
class LocalInputAudioNode;
}

namespace Controller {
    class MainController;
}

class LocalTrackView : public BaseTrackView
{
    Q_OBJECT
public:
    LocalTrackView(Controller::MainController* mainController, int channelIndex, float initialGain, BoostValue boostValue, float initialPan, bool muted);
    LocalTrackView(Controller::MainController* mainController, int channelIndex);
    ~LocalTrackView();

    void addPlugin(Audio::Plugin* plugin, bool bypassed=false);

    void closeAllPlugins();

    void refreshInputSelectionName();

    void detachMainController();

    inline int getInputIndex() const{return getTrackID();}

    void setToNoInput();
    Audio::LocalInputAudioNode* getInputNode() const;
    QList<const Audio::Plugin*> getInsertedPlugins() const;

    virtual void setUnlightStatus(bool unlighted);

    void setPeakMetersOnlyMode(bool peakMetersOnly, bool runningInMiniMode);
    void togglePeakMetersOnlyMode(bool runninsInMiniMode);

    QSize sizeHint() const;

    void updateGuiElements();//overriding to show midi activity

    //PRESETS NEED THAT TOO
    inline FxPanel* getFxPanel(){return fxPanel;}
    void resetFXPanel();
    void loadFXPanel();
    void mute(bool b);
    void solo(bool b);
    void initializeBoostButtons(BoostValue boostValue);

private:
    //int inputIndex;
    FxPanel* fxPanel;
    QSpacerItem* fxSpacer;
    FxPanel* createFxPanel();
    QWidget* createInputPanel();
    QPushButton* createInputSelectionButton(QWidget* parent);
    QLabel* createInputTypeIconLabel(QWidget *parent);

    QMenu* createMonoInputsMenu(QMenu* parentMenu);
    QMenu* createStereoInputsMenu(QMenu* parentMenu);
    QMenu* createMidiInputsMenu(QMenu* parentMenu);

    QPushButton* inputSelectionButton;
    QLabel* inputTypeIconLabel;

    QWidget* inputPanel;

    PeakMeter* midiPeakMeter;//show midi activity

    void setMidiPeakMeterVisibility(bool visible);

    static const QString MIDI_ICON;
    static const QString MONO_ICON;
    static const QString STEREO_ICON;
    static const QString NO_INPUT_ICON;

    QString getInputChannelNameOnly(int inputIndex);//return the input channel name withou the number/index

    Audio::LocalInputAudioNode* inputNode;

    bool inputIsUsedByThisTrack(int inputIndexInAudioDevice) const;

    void init(int channelIndex, float initialGain, BoostValue boostValue, float initialPan, bool muted);

    bool peakMetersOnly;



private slots:
    void on_inputSelectionButtonClicked();//build and show de input selection menu

    void on_monoInputMenuSelected(QAction*);
    void on_stereoInputMenuSelected(QAction*);
    void on_MidiInputDeviceSelected(QAction*);
    void on_noInputMenuSelected();
};

