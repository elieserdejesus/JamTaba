#pragma once

#include "BaseTrackView.h"
#include <QWidget>
#include <QTimer>

class QMenu;
class FxPanel;
class QFrame;
class QPushButton;
class QLabel;

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
    LocalTrackView(Controller::MainController* mainController, int channelIndex, float initialGain, float initialPan);
    LocalTrackView(Controller::MainController* mainController, int channelIndex);
    ~LocalTrackView();

    void addPlugin(Audio::Plugin* plugin, bool bypassed=false);
    void refreshInputSelectionName();

    inline int getInputIndex() const{return getTrackID();}

    void setToNoInput();
    Audio::LocalInputAudioNode* getInputNode() const;
    QList<const Audio::Plugin*> getInsertedPlugins() const;
private:
    //int inputIndex;
    FxPanel* fxPanel;
    FxPanel* createFxPanel();
    QWidget* createInputPanel();
    QPushButton* createInputSelectionButton(QWidget* parent);
    QLabel* createInputTypeIconLabel(QWidget *parent);

    QMenu* createMonoInputsMenu(QMenu* parentMenu);
    QMenu* createStereoInputsMenu(QMenu* parentMenu);
    QMenu* createMidiInputsMenu(QMenu* parentMenu);

    QPushButton* inputSelectionButton;
    QLabel* inputTypeIconLabel;

    static const QString MIDI_ICON;
    static const QString MONO_ICON;
    static const QString STEREO_ICON;
    static const QString NO_INPUT_ICON;

    QString getInputChannelNameOnly(int inputIndex);//return the input channel name withou the number/index

    Audio::LocalInputAudioNode* inputNode;

    bool inputIsUsedByMe(int inputIndexInAudioDevice) const;

    void init(int channelIndex, float initialGain, float initialPan);
private slots:
    void on_inputSelectionButtonClicked();

    void on_monoInputMenuSelected(QAction*);
    void on_stereoInputMenuSelected(QAction*);
    void on_MidiInputMenuSelected(QAction*);
    void on_noInputMenuSelected();
};

