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
}

namespace Controller {
    class MainController;
}

class LocalTrackView : public BaseTrackView
{
    Q_OBJECT
public:
    LocalTrackView(QWidget *parent, Controller::MainController* mainController);
    ~LocalTrackView();
    void initializeFxPanel(QMenu* fxMenu);
    void addPlugin(Audio::Plugin* plugin);
    void refreshInputSelectionName();
    inline Controller::MainController* getMainController() const{return mainController;}

signals:
    //plugins
    void editingPlugin(Audio::Plugin* plugi);
    void removingPlugin(Audio::Plugin* plugin);
private:

    FxPanel* fxPanel;
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

private slots:
    void on_inputSelectionButtonClicked();

    void on_monoInputMenuSelected(QAction*);
    void on_stereoInputMenuSelected(QAction*);
    void on_MidiInputMenuSelected(QAction*);
    void on_noInputMenuSelected();
};

