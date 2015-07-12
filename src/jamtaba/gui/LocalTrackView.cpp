#include "LocalTrackView.h"
#include "ui_BaseTrackView.h"
#include "FxPanel.h"
#include "FxPanelItem.h"
#include "plugins/guis.h"
#include "../MainController.h"
#include "../midi/MidiDriver.h"
#include "../audio/core/AudioDriver.h"
#include "../audio/core/AudioNode.h"
#include <QMenu>
#include <QToolButton>

const QString LocalTrackView::MIDI_ICON = ":/images/input_midi.png";
const QString LocalTrackView::MONO_ICON = ":/images/input_mono.png";
const QString LocalTrackView::STEREO_ICON = ":/images/input_stereo.png";
const QString LocalTrackView::NO_INPUT_ICON= ":/images/input_no.png";

LocalTrackView::LocalTrackView(QWidget* parent, Controller::MainController *mainController)
    :BaseTrackView(parent, mainController, 1), fxPanel(nullptr)
{
    //create input panel in the bottom
    ui->mainLayout->addSpacing(20);
    ui->mainLayout->addWidget(createInputPanel());
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QWidget* LocalTrackView::createInputPanel(){
    QWidget* inputPanel = new QWidget(this);
    inputPanel->setObjectName("inputPanel");
    inputPanel->setLayout(new QHBoxLayout(inputPanel));
    inputPanel->layout()->setContentsMargins(0, 0, 0, 0);
    inputPanel->layout()->setSpacing(3);
    inputPanel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

    this->inputTypeIconLabel = createInputTypeIconLabel(inputPanel);
    this->inputSelectionButton = createInputSelectionButton(inputPanel);
    inputPanel->layout()->addWidget(inputTypeIconLabel);
    inputPanel->layout()->addWidget(inputSelectionButton);//button in right
    return inputPanel;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QLabel* LocalTrackView::createInputTypeIconLabel(QWidget *parent){
    QLabel* label = new QLabel(parent);
    label->setObjectName("inputSelectionIcon");
    label->setTextFormat(Qt::RichText);
    label->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
    return label;
}

QPushButton* LocalTrackView::createInputSelectionButton(QWidget* parent){
    QPushButton* fakeComboButton = new QPushButton(parent);
    fakeComboButton->setObjectName("inputSelectionButton");
    fakeComboButton->setText("inputs ...");
    fakeComboButton->setToolTip("Choose input channels ...");
    fakeComboButton->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    QObject::connect( fakeComboButton, SIGNAL(clicked()), this, SLOT(on_inputSelectionButtonClicked()));

    return fakeComboButton;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::on_inputSelectionButtonClicked(){
    QMenu menu(this);
    //menu.setParent(inputSelectionButton);
    menu.move(mapToGlobal(inputSelectionButton->parentWidget()->pos()));
    menu.addMenu(createMonoInputsMenu(&menu));
    menu.addMenu(createStereoInputsMenu(&menu));
    menu.addMenu(createMidiInputsMenu(&menu));
    QAction* noInputAction = menu.addAction(QIcon(NO_INPUT_ICON), "no input");
    QObject::connect( noInputAction, SIGNAL(triggered()), this, SLOT(on_noInputMenuSelected()));
    menu.exec();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::on_noInputMenuSelected(){
    mainController->setInputTrackToNoInput();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QMenu* LocalTrackView::createMonoInputsMenu(QMenu* parent){
    QMenu* monoInputsMenu = new QMenu("Mono", parent);
    monoInputsMenu->setIcon(QIcon(MONO_ICON));
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    Audio::ChannelRange inputsRange = audioDriver->getSelectedInputs();
    int inputs = inputsRange.getChannels();
    int firstInputIndex = inputsRange.getFirstChannel();
    QString deviceName(audioDriver->getInputDeviceName(audioDriver->getInputDeviceIndex()));
    for (int i = 0; i < inputs; ++i) {
        int index = firstInputIndex + i;
        QString inputName = QString(audioDriver->getInputChannelName(index)) + "  (" + deviceName + ")";
        QAction* action = monoInputsMenu->addAction(inputName);
        action->setData( index );//using the channel index as action data
        action->setIcon(monoInputsMenu->icon());
    }
    QObject::connect( monoInputsMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_monoInputMenuSelected(QAction*)));
    return monoInputsMenu;
}

void LocalTrackView::on_monoInputMenuSelected(QAction *action){
    int selectedInputIndex = action->data().toInt();
    mainController->setInputTrackToMono(selectedInputIndex);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QMenu* LocalTrackView::createStereoInputsMenu(QMenu* parent){
    QMenu* stereoInputsMenu = new QMenu("Stereo", parent);
    stereoInputsMenu->setIcon(QIcon(STEREO_ICON));
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    Audio::ChannelRange inputsRange = audioDriver->getSelectedInputs();
    int inputs = inputsRange.getChannels();
    QString deviceName(audioDriver->getInputDeviceName(audioDriver->getInputDeviceIndex()));
    for (int i = 0; i < inputs; i += 2) {
        if(i + 1 < inputs){//we can make a channel pair using (i) and (i+1)?
            int index = inputsRange.getFirstChannel() + i;
            QString firstName = getInputChannelNameOnly(index);
            QString indexes = QString::number(index+1) + " + " + QString::number(index+2);
            QString inputName = firstName + " [" + indexes +  "]  (" + deviceName + ")";
            QAction* action = stereoInputsMenu->addAction(inputName);
            action->setData(index);//use the first input pair index as action data
            action->setIcon(stereoInputsMenu->icon());
        }
    }
    stereoInputsMenu->setEnabled(inputs/2 >= 1);//at least one pair
    if(!stereoInputsMenu->isEnabled()){
        stereoInputsMenu->setTitle( stereoInputsMenu->title() + "  (not enough inputs to make stereo)" );
    }
    QObject::connect(stereoInputsMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_stereoInputMenuSelected(QAction*)));
    return stereoInputsMenu;
}

void LocalTrackView::on_stereoInputMenuSelected(QAction *action){
    int firstInputIndex = action->data().toInt();
    mainController->setInputTrackToStereo(firstInputIndex);
}

QString LocalTrackView::getInputChannelNameOnly(int inputIndex){
    QString fullName(mainController->getAudioDriver()->getInputChannelName(inputIndex));
    int spaceIndex = fullName.lastIndexOf(" ");
    if(spaceIndex > 0){
        return fullName.left( spaceIndex );
    }
    return fullName;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::refreshInputSelectionName(){
    Audio::LocalInputAudioNode* inputTrack = dynamic_cast<Audio::LocalInputAudioNode*>( mainController->getTrackNode(Controller::MainController::INPUT_TRACK_ID));
    QString channelName;
    QString iconFile;
    if(!inputTrack->isMidi()){//using audio as input method
        Audio::ChannelRange inputRange = inputTrack->getAudioInputRange();
        if(inputTrack->isStereo()){
            int firstInputIndex = inputRange.getFirstChannel();
            QString indexes = "(" + QString::number(firstInputIndex+1) + "+" + QString::number(firstInputIndex+2) + ") ";
            channelName = indexes + getInputChannelNameOnly(firstInputIndex);
            iconFile = STEREO_ICON;
        }
        else if(inputTrack->isMono()){
            int index = inputRange.getFirstChannel();
            channelName = QString(QString::number(index+1) + " - ") + mainController->getAudioDriver()->getInputChannelName(index);
            iconFile = MONO_ICON;
        }
        else{//range is empty = no audio input
            channelName = "No input";
            iconFile = NO_INPUT_ICON;
        }
    }
    else{//using midi as input method
        channelName = mainController->getMidiDriver()->getInputDeviceName(inputTrack->getMidiDeviceIndex());
        iconFile = MIDI_ICON;
    }

    //set the input name
    QString elidedName = inputSelectionButton->fontMetrics().elidedText(channelName, Qt::ElideRight, inputSelectionButton->width() - inputSelectionButton->iconSize().width());
    inputSelectionButton->setText(elidedName);

    //set the icon
    this->inputTypeIconLabel->setText("<img src=" + iconFile + "/>");

    update();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QMenu* LocalTrackView::createMidiInputsMenu(QMenu* parent){
    QMenu* midiInputsMenu = new QMenu("MIDI", parent);
    midiInputsMenu->setIcon(QIcon(MIDI_ICON));
    int midiDevices = mainController->getMidiDriver()->getMaxInputDevices();
    for (int d = 0; d < midiDevices; ++d) {
        QAction* action = midiInputsMenu->addAction(QString(mainController->getMidiDriver()->getInputDeviceName(d)));
        action->setData(d);//using midi device index as action data
        action->setIcon(midiInputsMenu->icon());
    }
    midiInputsMenu->setEnabled(midiDevices > 0);
    if(!midiInputsMenu->isEnabled()){
        midiInputsMenu->setTitle( midiInputsMenu->title() + "  (no MIDI device detected)" );
    }
    QObject::connect(midiInputsMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_MidiInputMenuSelected(QAction*)));
    return midiInputsMenu;
}

void LocalTrackView::on_MidiInputMenuSelected(QAction *action){
    int midiDeviceIndex = action->data().toInt();
    mainController->setInputTrackToMIDI(midiDeviceIndex);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::addPlugin(Audio::Plugin* plugin){
    if(fxPanel){
        this->fxPanel->addPlugin(plugin);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::initializeFxPanel(QMenu *fxMenu){
    if(fxMenu && !fxMenu->isEmpty()){
        if(!fxPanel){
            fxPanel = new FxPanel(this);
            fxPanel->connect(fxPanel, SIGNAL(editingPlugin(Audio::Plugin*)), this, SIGNAL(editingPlugin(Audio::Plugin*)));
            fxPanel->connect(fxPanel, SIGNAL(pluginRemoved(Audio::Plugin*)), this, SIGNAL(removingPlugin(Audio::Plugin*)));

            int index = ui->mainLayout->indexOf(ui->panSpacer->widget());

            ui->mainLayout->insertWidget( index+1, fxPanel);

            //add separator after effects panel
            ui->mainLayout->insertSpacing( index + 2, 20);
        }
        fxPanel->setFxMenu(fxMenu);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LocalTrackView::~LocalTrackView()
{

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
