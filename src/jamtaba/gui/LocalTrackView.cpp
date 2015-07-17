#include "LocalTrackView.h"
#include "ui_BaseTrackView.h"
#include "FxPanel.h"
#include "FxPanelItem.h"
#include "plugins/guis.h"
#include "../MainController.h"
#include "../midi/MidiDriver.h"
#include "../audio/core/AudioDriver.h"
#include "../audio/core/AudioNode.h"
#include "Highligther.h"
#include <QMenu>
#include <QToolButton>

const QString LocalTrackView::MIDI_ICON = ":/images/input_midi.png";
const QString LocalTrackView::MONO_ICON = ":/images/input_mono.png";
const QString LocalTrackView::STEREO_ICON = ":/images/input_stereo.png";
const QString LocalTrackView::NO_INPUT_ICON= ":/images/input_no.png";

LocalTrackView::LocalTrackView(QWidget* parent, Controller::MainController *mainController, float initialGain, float initialPan)
    :BaseTrackView(parent, mainController, 1), fxPanel(nullptr), inputNode(nullptr)
{
    init(initialGain, initialPan);
}

LocalTrackView::LocalTrackView(QWidget* parent, Controller::MainController *mainController)
    :BaseTrackView(parent, mainController, 1), fxPanel(nullptr), inputNode(nullptr)
{
    init(1, 0);//unit gain and pan in center
}

void LocalTrackView::init(float initialGain, float initialPan){
    //add separator before effects panel
    ui->mainLayout->addSpacing(20);
    fxPanel = createFxPanel();
    ui->mainLayout->addWidget( fxPanel );

    //create input panel in the bottom
    ui->mainLayout->addSpacing(20);
    ui->mainLayout->addWidget(createInputPanel());

    //insert a input node in controller
    this->inputNode = new Audio::LocalInputAudioNode();
    this->trackID = mainController->addInputTrackNode(this->inputNode);

    ui->levelSlider->setValue( 100 * initialGain );
    ui->panSlider->setValue( initialPan * 4 );
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Audio::LocalInputAudioNode* LocalTrackView::getInputNode() const{
    return inputNode;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::setToNoInput(){
    if(inputNode){
        inputNode->setToNoInput();
        refreshInputSelectionName();
    }

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::updatePeaks(){
    Audio::AudioPeak peak = mainController->getTrackPeak(getTrackID());
    setPeaks(peak.getLeft(), peak.getRight());
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
    menu.addMenu(createMonoInputsMenu(&menu));
    menu.addMenu(createStereoInputsMenu(&menu));
    menu.addMenu(createMidiInputsMenu(&menu));
    QAction* noInputAction = menu.addAction(QIcon(NO_INPUT_ICON), "no input");
    QObject::connect( noInputAction, SIGNAL(triggered()), this, SLOT(on_noInputMenuSelected()));

    menu.move(mapToGlobal(inputSelectionButton->parentWidget()->pos()));
    menu.exec();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::on_noInputMenuSelected(){
    mainController->setInputTrackToNoInput(getTrackID());
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QMenu* LocalTrackView::createMonoInputsMenu(QMenu* parent){
    QMenu* monoInputsMenu = new QMenu("Mono", parent);
    monoInputsMenu->setIcon(QIcon(MONO_ICON));
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    Audio::ChannelRange globalInputsRange = audioDriver->getSelectedInputs();
    int globalInputs = globalInputsRange.getChannels();
    int firstGlobalInputIndex = globalInputsRange.getFirstChannel();
    QString deviceName(audioDriver->getInputDeviceName(audioDriver->getInputDeviceIndex()));
    //Audio::ChannelRange thisTrackInputRange = inputNode->getAudioInputRange();
    //bool canEnableMenu = false;
    for (int i = 0; i < globalInputs; ++i) {
        int index = firstGlobalInputIndex + i;
        QString inputName = QString(audioDriver->getInputChannelName(index)) + "  (" + deviceName + ")";
        QAction* action = monoInputsMenu->addAction(inputName);
        action->setData( index );//using the channel index as action data
        action->setIcon(monoInputsMenu->icon());
//        action->setEnabled(mainController->audioMonoInputIsFreeToSelect(index) || index == thisTrackInputRange.getFirstChannel() || index == thisTrackInputRange.getLastChannel());
//        if(action->isEnabled()){//at least one action is enabled
//            canEnableMenu = true;
//        }
    }
//    monoInputsMenu->setEnabled(canEnableMenu);
//    if(!canEnableMenu){
//        monoInputsMenu->setTitle("Mono (all mono inputs are already in use)");
//    }
    QObject::connect( monoInputsMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_monoInputMenuSelected(QAction*)));
    return monoInputsMenu;
}

void LocalTrackView::on_monoInputMenuSelected(QAction *action){
    int selectedInputIndexInAudioDevice = action->data().toInt();
    mainController->setInputTrackToMono(getTrackID(), selectedInputIndexInAudioDevice);

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QMenu* LocalTrackView::createStereoInputsMenu(QMenu* parent){
    QMenu* stereoInputsMenu = new QMenu("Stereo", parent);
    stereoInputsMenu->setIcon(QIcon(STEREO_ICON));
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    Audio::ChannelRange globalInputRange = audioDriver->getSelectedInputs();
    //Audio::ChannelRange thisTrackInputRange = mainController->getInputTrack(getTrackID())->getAudioInputRange();
    int globalInputs = globalInputRange.getChannels();
    QString deviceName(audioDriver->getInputDeviceName(audioDriver->getInputDeviceIndex()));
    //bool canEnableMenu = false;
    for (int i = 0; i < globalInputs; i += 2) {
        if(i + 1 < globalInputs){//we can make a channel pair using (i) and (i+1)?
            int index = globalInputRange.getFirstChannel() + i;
            QString firstName = getInputChannelNameOnly(index);
            QString indexes = QString::number(index+1) + " + " + QString::number(index+2);
            QString inputName = firstName + " [" + indexes +  "]  (" + deviceName + ")";
            QAction* action = stereoInputsMenu->addAction(inputName);
            action->setData(index);//use the first input pair index as action data
            action->setIcon(stereoInputsMenu->icon());
//            bool inputPairIsFree = mainController->audioStereoInputIsFreeToSelect(index);
//            bool inputUsageIsOk = inputIsUsedByMe(index) || (inputIsUsedByMe(index+1) && thisTrackInputRange.isMono());
//            action->setEnabled( inputPairIsFree || inputUsageIsOk );
//            if(action->isEnabled()){
//                canEnableMenu = true;
//            }
        }
    }
    stereoInputsMenu->setEnabled(globalInputs/2 >= 1);//at least one pair
    if(!stereoInputsMenu->isEnabled()){
        stereoInputsMenu->setTitle( stereoInputsMenu->title() + "  (not enough available inputs to make stereo)" );
    }
    QObject::connect(stereoInputsMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_stereoInputMenuSelected(QAction*)));
    return stereoInputsMenu;
}

//bool LocalTrackView::inputIsUsedByMe(int inputIndexInAudioDevice) const{
//    Audio::ChannelRange range = inputNode->getAudioInputRange();
//    return range.getFirstChannel() == inputIndexInAudioDevice || range.getLastChannel() == inputIndexInAudioDevice;
//}

void LocalTrackView::on_stereoInputMenuSelected(QAction *action){
    int firstInputIndexInAudioDevice = action->data().toInt();
    mainController->setInputTrackToStereo(getTrackID(), firstInputIndexInAudioDevice);

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
    Audio::LocalInputAudioNode* inputTrack = mainController->getInputTrack(getTrackID());
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
        midiInputsMenu->setTitle( midiInputsMenu->title() + "  (no MIDI devices detected)" );
    }
    QObject::connect(midiInputsMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_MidiInputMenuSelected(QAction*)));
    return midiInputsMenu;
}

void LocalTrackView::on_MidiInputMenuSelected(QAction *action){
    int midiDeviceIndex = action->data().toInt();
    mainController->setInputTrackToMIDI(getTrackID(), midiDeviceIndex);

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::addPlugin(Audio::Plugin* plugin, bool bypassed){
    if(fxPanel){
        plugin->setEditor(new Audio::PluginWindow(plugin));
        plugin->setBypass(bypassed);
        this->fxPanel->addPlugin(plugin);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FxPanel *LocalTrackView::createFxPanel(){
    FxPanel* panel = new FxPanel(this, mainController);

    //panel->connect(panel, SIGNAL(editingPlugin(Audio::Plugin*)), this, SIGNAL(editingPlugin(Audio::Plugin*)));
    //panel->connect(panel, SIGNAL(pluginRemoved(Audio::Plugin*)), this, SIGNAL(removingPlugin(Audio::Plugin*)));
    return panel;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LocalTrackView::~LocalTrackView()
{
    mainController->removeInputTrackNode(getTrackID());
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QList<const Audio::Plugin *> LocalTrackView::getInsertedPlugins() const{
    QList<const Audio::Plugin*> plugins;
    foreach (FxPanelItem* item, fxPanel->getItems()) {
        if(item->containPlugin() ){
            plugins.append(item->getAudioPlugin());
        }
    }
    return plugins;
}
