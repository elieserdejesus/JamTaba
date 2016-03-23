#include "LocalTrackViewStandalone.h"
#include "audio/core/LocalInputNode.h"
#include "FxPanel.h"
#include "FxPanelItem.h"
#include <QGridLayout>
#include <QStyle>
#include <QDesktopWidget>

const QString LocalTrackViewStandalone::MIDI_ICON = ":/images/input_midi.png";
const QString LocalTrackViewStandalone::MONO_ICON = ":/images/input_mono.png";
const QString LocalTrackViewStandalone::STEREO_ICON = ":/images/input_stereo.png";
const QString LocalTrackViewStandalone::NO_INPUT_ICON = ":/images/input_no.png";

LocalTrackViewStandalone::LocalTrackViewStandalone(
    Controller::MainControllerStandalone *mainController, int channelIndex) :
    LocalTrackView(mainController, channelIndex),
    controller(mainController),
    midiToolsDialog(nullptr)
{
    fxPanel = createFxPanel();
    //mainLayout->addSpacing(20);// add separator before effects panel
    mainLayout->addWidget(fxPanel, mainLayout->rowCount(), 0, 1, 2);

    // create input panel in the bottom
    this->inputPanel = createInputPanel();

    mainLayout->addWidget(inputPanel, mainLayout->rowCount(), 0, 1, 2);

    midiToolsButton = createMidiToolsButton();
    mainLayout->addWidget(midiToolsButton, mainLayout->rowCount(), 0, 1, 2);
    midiToolsButton->setVisible(getInputNode()->isMidi());

    this->inputTypeIconLabel = createInputTypeIconLabel(this);
    mainLayout->addWidget(inputTypeIconLabel, mainLayout->rowCount(), 0, 1, 2);
    mainLayout->setAlignment(this->inputTypeIconLabel, Qt::AlignCenter);

    // create the peak meter to show midiactivity
    midiPeakMeter = new PeakMeter();
    midiPeakMeter->setObjectName("midiPeakMeter");
    midiPeakMeter->setSolidColor(QColor(180, 0, 0));
    midiPeakMeter->setPaintMaxPeakMarker(false);
    midiPeakMeter->setDecayTime(500);// 500 ms
    midiPeakMeter->setAccessibleDescription("This is the midi activity meter");
}

void LocalTrackViewStandalone::updateGuiElements()
{
    LocalTrackView::updateGuiElements();

    if (inputNode && inputNode->hasMidiActivity()) {
        quint8 midiActivityValue = inputNode->getMidiActivityValue();
        midiPeakMeter->setPeak(midiActivityValue/127.0);
        inputNode->resetMidiActivity();
    }
    if (midiPeakMeter->isVisible())
        midiPeakMeter->update();
}

void LocalTrackViewStandalone::reset()
{
    LocalTrackView::reset();
    if (fxPanel)
        fxPanel->removePlugins();
}

QLabel *LocalTrackViewStandalone::createInputTypeIconLabel(QWidget *parent)
{
    QLabel *label = new QLabel(parent);
    label->setObjectName("inputSelectionIcon");
    label->setTextFormat(Qt::RichText);
    label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));
    return label;
}

QPushButton *LocalTrackViewStandalone::createInputSelectionButton(QWidget *parent)
{
    QPushButton *fakeComboButton = new QPushButton(parent);
    fakeComboButton->setObjectName("inputSelectionButton");
    fakeComboButton->setText("inputs ...");
    fakeComboButton->setToolTip("Choose input channels ...");
    fakeComboButton->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                               QSizePolicy::MinimumExpanding));

    QObject::connect(fakeComboButton, SIGNAL(clicked()), this, SLOT(showInputSelectionMenu()));

    return fakeComboButton;
}

QWidget *LocalTrackViewStandalone::createInputPanel()
{
    QWidget *inputPanel = new QWidget(this);
    inputPanel->setObjectName("inputPanel");
    inputPanel->setLayout(new QHBoxLayout(inputPanel));
    inputPanel->layout()->setContentsMargins(0, 0, 0, 0);
    inputPanel->layout()->setSpacing(0);
    inputPanel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

    this->inputSelectionButton = createInputSelectionButton(inputPanel);
    inputPanel->layout()->addWidget(inputSelectionButton);// button in right

    return inputPanel;
}

QPushButton *LocalTrackViewStandalone::createMidiToolsButton()
{
    QPushButton * button = new QPushButton();
    button->setObjectName("midiToolsButton");
    button->setText("MIDI Tools");
    connect(button, SIGNAL(clicked(bool)), this, SLOT(openMidiToolsDialog()));
    return button;
}

void LocalTrackViewStandalone::openMidiToolsDialog()
{
    if (!midiToolsDialog) {
        qCDebug(jtGUI) << "Creating a new MidiToolsDialog!";
        Audio::LocalInputNode *inputNode = getInputNode();
        QString higherNote = getMidiNoteText(inputNode->getMidiHigherNote());
        QString lowerNote = getMidiNoteText(inputNode->getMidiLowerNote());
        midiToolsDialog = new MidiToolsDialog(lowerNote, higherNote);
        connect(midiToolsDialog, &MidiToolsDialog::dialogClosed, this, &LocalTrackViewStandalone::clearMidiToolsDialog);
        connect(midiToolsDialog, &MidiToolsDialog::lowerNoteChanged, this, &LocalTrackViewStandalone::setMidiLowerNote);
        connect(midiToolsDialog, &MidiToolsDialog::higherNoteChanged, this, &LocalTrackViewStandalone::setMidiHigherNote);
        connect(midiToolsDialog, &MidiToolsDialog::transposeChanged, this, &LocalTrackViewStandalone::setTranspose);
    }

    QRect desktopRect = QApplication::desktop()->availableGeometry();

    QPoint point = mapToGlobal(QPoint(x() + width(), inputPanel->y()));

    if (point.y() + midiToolsDialog->height() > desktopRect.height()) {
        point.setY( desktopRect.height() - (midiToolsDialog->height() + 35)); //align in bottom with 35 pixels in margim
    }

    midiToolsDialog->move(point);
    midiToolsDialog->show();
    midiToolsDialog->raise();
}

void LocalTrackViewStandalone::setTranspose(qint8 transposeValue)
{
     Audio::LocalInputNode *inputNode = getInputNode();
     if (inputNode) {
         inputNode->setTranspose(transposeValue);
     }
}

void LocalTrackViewStandalone::setMidiHigherNote(const QString &higherNote)
{
    Audio::LocalInputNode *inputNode = getInputNode();
    if (inputNode) {
        quint8 noteNumber = getMidiNoteNumber(higherNote);
        inputNode->setMidiHigherNote(noteNumber);
    }
}

void LocalTrackViewStandalone::setMidiLowerNote(const QString &lowerNote)
{
    Audio::LocalInputNode *inputNode = getInputNode();
    if (inputNode) {
        quint8 noteNumber = getMidiNoteNumber(lowerNote);
        inputNode->setMidiLowerNote(noteNumber);
    }
}

QString LocalTrackViewStandalone::getMidiNoteText(quint8 midiNoteNumber) const
{
    int octave = midiNoteNumber/12;
    QString noteName = "";
    switch (midiNoteNumber % 12) {
        case 0: noteName = "C"; break;
        case 1: noteName = "C#"; break;
        case 2: noteName = "D"; break;
        case 3: noteName = "D#"; break;
        case 4: noteName = "E"; break;
        case 5: noteName = "F"; break;
        case 6: noteName = "F#"; break;
        case 7: noteName = "G"; break;
        case 8: noteName = "G#"; break;
        case 9: noteName = "A"; break;
        case 10: noteName = "A#"; break;
        case 11: noteName = "B"; break;
    }
    return noteName + QString::number(octave);
}

quint8 LocalTrackViewStandalone::getMidiNoteNumber(const QString &midiNote) const
{
    //midi note numbers: http://www.midimountain.com/midi/midi_note_numbers.html

    if (midiNote.length() < 2)
        return -1;

     quint8 noteNumber = 0;
     switch (midiNote.at(0).toLatin1()) {
        case 'C': noteNumber = 0; break;
        case 'D': noteNumber = 2; break;
        case 'E': noteNumber = 4; break;
        case 'F': noteNumber = 5; break;
        case 'G': noteNumber = 7; break;
        case 'A': noteNumber = 9; break;
        case 'B': noteNumber = 11; break;
     default: noteNumber = 0;
         break;
     }
     int octave = midiNote.at( (midiNote.length() > 2) ? 2 : 1).toLatin1() - 48; //ascii to int

     noteNumber += (octave * 12);

     if (midiNote.contains('b'))
         noteNumber--;
     if (midiNote.contains('#'))
         noteNumber++;

     return noteNumber;
}

void LocalTrackViewStandalone::clearMidiToolsDialog()
{
    midiToolsDialog = nullptr;
    qCDebug(jtGUI) << "MidiToolsDialog pointer cleared!";
}

void LocalTrackViewStandalone::addPlugin(Audio::Plugin *plugin, bool bypassed)
{
    if (fxPanel) {
        plugin->setBypass(bypassed);
        this->fxPanel->addPlugin(plugin);
        this->refreshInputSelectionName();// refresh input type combo box, if the added plugins is a virtual instrument Jamtaba will try auto change the input type to midi
        update();
    }
}

QList<const Audio::Plugin *> LocalTrackViewStandalone::getInsertedPlugins() const
{
    QList<const Audio::Plugin *> plugins;
    if (fxPanel) {// can be nullptr in vst plugin
        foreach (FxPanelItem *item, fxPanel->getItems()) {
            if (item->containPlugin())
                plugins.append(item->getAudioPlugin());
        }
    }
    return plugins;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FxPanel *LocalTrackViewStandalone::createFxPanel()
{
    return new FxPanel(this, controller);
}

void LocalTrackViewStandalone::setUnlightStatus(bool unlighted)
{
    LocalTrackView::setUnlightStatus(unlighted);

    if (fxPanel) {
        style()->unpolish(fxPanel);
        style()->polish(fxPanel);

        QList<FxPanelItem *> items = fxPanel->getItems();
        foreach (FxPanelItem *item, items) {
            style()->unpolish(item);
            style()->polish(item);
        }
    }

    if (inputPanel) {
        style()->unpolish(inputPanel);
        style()->polish(inputPanel);
    }
}

void LocalTrackViewStandalone::setPeakMetersOnlyMode(bool peakMetersOnly, bool runningInMiniMode)
{
    LocalTrackView::setPeakMetersOnlyMode(peakMetersOnly, runningInMiniMode);

    if (fxPanel) {// in vst plugin fxPanel is nullptr
        fxPanel->setVisible(!peakMetersOnly);
        inputPanel->setVisible(!peakMetersOnly);
    }

    if (inputTypeIconLabel)// this will be nullptr in Vst plugin
        inputTypeIconLabel->setVisible(!peakMetersOnly);
}

QMenu *LocalTrackViewStandalone::createMonoInputsMenu(QMenu *parent)
{
    QMenu *monoInputsMenu = new QMenu("Mono", parent);
    monoInputsMenu->setIcon(QIcon(MONO_ICON));
    Audio::AudioDriver *audioDriver = controller->getAudioDriver();
    int globalInputs = audioDriver->getInputsCount();
    QString deviceName(audioDriver->getAudioDeviceName(audioDriver->getAudioDeviceIndex()));

    for (int i = 0; i < globalInputs; ++i) {
        int index = audioDriver->getFirstSelectedInput() + i;
        QString channelName = QString(audioDriver->getInputChannelName(index)).trimmed();
        if (channelName.isNull() || channelName.isEmpty())
            channelName = QString::number(index+1)  + " "+ audioDriver->getAudioDeviceName();
        QString inputName = channelName + "  (" + deviceName + ")";
        QAction *action = monoInputsMenu->addAction(inputName);
        action->setData(i);  // using the channel index as action data
        action->setIcon(monoInputsMenu->icon());
    }

    QObject::connect(monoInputsMenu, SIGNAL(triggered(QAction *)), this, SLOT(setToMono(QAction *)));
    return monoInputsMenu;
}

void LocalTrackViewStandalone::showInputSelectionMenu()
{
    QMenu menu(this);
    menu.addMenu(createMonoInputsMenu(&menu));
    menu.addMenu(createStereoInputsMenu(&menu));
    menu.addMenu(createMidiInputsMenu(&menu));
    QAction *noInputAction = menu.addAction(QIcon(NO_INPUT_ICON), "no input");
    QObject::connect(noInputAction, SIGNAL(triggered()), this, SLOT(setToNoInput()));

    menu.move(mapToGlobal(inputSelectionButton->parentWidget()->pos()));
    menu.exec();
}

QMenu *LocalTrackViewStandalone::createStereoInputsMenu(QMenu *parent)
{
    QMenu *stereoInputsMenu = new QMenu("Stereo", parent);
    stereoInputsMenu->setIcon(QIcon(STEREO_ICON));
    Audio::AudioDriver *audioDriver = controller->getAudioDriver();
    int globalInputs = audioDriver->getInputsCount();
    QString deviceName(audioDriver->getAudioDeviceName(audioDriver->getAudioDeviceIndex()));
    for (int i = 0; i < globalInputs; i += 2) {
        if (i + 1 < globalInputs) {// we can make a channel pair using (i) and (i+1)?
            QString firstName = getInputChannelNameOnly(i);
            QString indexes = QString::number(i+1) + " + " + QString::number(i+2);
            QString inputName = firstName + " [" + indexes +  "]  (" + deviceName + ")";
            QAction *action = stereoInputsMenu->addAction(inputName);
            action->setData(i);// use the first input pair index as action data
            action->setIcon(stereoInputsMenu->icon());
        }
    }
    stereoInputsMenu->setEnabled(globalInputs/2 >= 1);// at least one pair
    if (!stereoInputsMenu->isEnabled()) {
        QString msg = stereoInputsMenu->title() + "  (not enough available inputs to make stereo)";
        stereoInputsMenu->setTitle(msg);
    }
    QObject::connect(stereoInputsMenu, SIGNAL(triggered(QAction *)), this, SLOT(setToStereo(QAction *)));
    return stereoInputsMenu;
}

QString LocalTrackViewStandalone::getInputChannelNameOnly(int inputIndex)
{
    Audio::AudioDriver *audioDriver = controller->getAudioDriver();
    QString fullName(audioDriver->getInputChannelName(inputIndex));
    if (fullName.isEmpty())// mac return empy channel names if user don't rename the channels
        fullName = audioDriver->getAudioDeviceName();
    int spaceIndex = fullName.lastIndexOf(" ");
    if (spaceIndex > 0)
        return fullName.left(spaceIndex);
    return fullName;
}

QMenu *LocalTrackViewStandalone::createMidiInputsMenu(QMenu *parent)
{
    QMenu *midiInputsMenu = new QMenu("MIDI", parent);
    midiInputsMenu->setIcon(QIcon(MIDI_ICON));
    Midi::MidiDriver *midiDriver = controller->getMidiDriver();
    int totalMidiDevices = midiDriver->getMaxInputDevices();
    int globallyEnabledMidiDevices = 0;
    for (int d = 0; d < totalMidiDevices; ++d) {
        if (midiDriver->deviceIsGloballyEnabled(d)) {
            globallyEnabledMidiDevices++;
            QMenu *midiChannelsMenu = new QMenu(midiInputsMenu);
            QActionGroup *actionGroup = new QActionGroup(midiChannelsMenu);

            QAction *allChannelsAction = midiChannelsMenu->addAction("All channels");
            allChannelsAction->setData(QString(QString::number(d) + ":" + QString::number(-1)));// use -1 to all channels
            allChannelsAction->setActionGroup(actionGroup);
            allChannelsAction->setCheckable(true);
            Audio::LocalInputNode *inputNode = getInputNode();
            allChannelsAction->setChecked(
                inputNode->isMidi() && inputNode->getMidiDeviceIndex() == d
                && inputNode->isReceivingAllMidiChannels());

            midiChannelsMenu->addSeparator();
            for (int c = 0; c < 16; ++c) {
                QAction *a = midiChannelsMenu->addAction("Channel " + QString::number(c+1));
                a->setData(QString(QString::number(d) + ":" + QString::number(c)));// use device:channel_index as data
                a->setActionGroup(actionGroup);
                a->setCheckable(true);
                a->setChecked(
                    inputNode->isMidi() && inputNode->getMidiChannelIndex() == c
                    && inputNode->getMidiDeviceIndex() == d);
            }

            QString deviceName = controller->getMidiDriver()->getInputDeviceName(d);
            QAction *action = midiInputsMenu->addAction(deviceName);
            action->setMenu(midiChannelsMenu);
            action->setData(d);// using midi device index as action data
            action->setIcon(midiInputsMenu->icon());
            QObject::connect(midiChannelsMenu, SIGNAL(triggered(QAction *)), this,
                             SLOT(setToMidi(QAction *)));
        }
    }
    midiInputsMenu->setEnabled(globallyEnabledMidiDevices > 0);
    if (!midiInputsMenu->isEnabled()) {
        midiInputsMenu->setTitle(
            midiInputsMenu->title()
            + "  (no MIDI devices detected or enabled in 'Preferences' menu')");
    }
    return midiInputsMenu;
}

void LocalTrackViewStandalone::setToNoInput()
{
    if (inputNode) {
        inputNode->setToNoInput();
        midiToolsButton->setVisible(false);
        refreshInputSelectionName();
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void LocalTrackViewStandalone::refreshInputSelectionName()
{
    Audio::LocalInputNode *inputTrack = controller->getInputTrack(getTrackID());
    QString channelName;
    QString iconFile;
    if (inputTrack->isAudio()) {// using audio as input method
        Audio::ChannelRange inputRange = inputTrack->getAudioInputRange();
        if (inputTrack->isStereo()) {
            int firstInputIndex = inputRange.getFirstChannel() + controller->getAudioDriver()->getFirstSelectedInput();
            QString indexes = "(" + QString::number(firstInputIndex+1) + "+" + QString::number(
                firstInputIndex+2) + ") ";
            channelName = indexes + getInputChannelNameOnly(firstInputIndex);
            iconFile = STEREO_ICON;
        } else if (inputTrack->isMono()) {
            Audio::AudioDriver *audioDriver = controller->getAudioDriver();
            int index = inputRange.getFirstChannel() + controller->getAudioDriver()->getFirstSelectedInput();
            QString name = QString(audioDriver->getInputChannelName(index));
            channelName = QString(QString::number(index+1) + " - ");
            if (!name.isNull() && !name.isEmpty())
                channelName += name;
            else
                channelName += QString(audioDriver->getAudioDeviceName());

            iconFile = MONO_ICON;
        } else {// range is empty = no audio input
            channelName = "No input";
            iconFile = NO_INPUT_ICON;
        }
    } else {
        if (inputTrack->isMidi()) {
            Midi::MidiDriver *midiDriver = controller->getMidiDriver();
            int selectedDeviceIndex = inputTrack->getMidiDeviceIndex();
            if (selectedDeviceIndex < midiDriver->getMaxInputDevices()
                && midiDriver->deviceIsGloballyEnabled(selectedDeviceIndex)) {
                channelName = midiDriver->getInputDeviceName(selectedDeviceIndex);
                iconFile = MIDI_ICON;
            } else {// midi device index invalid
                inputTrack->setToNoInput();
                channelName = "No input";
                iconFile = NO_INPUT_ICON;
            }
        } else {
            channelName = "No input";
            iconFile = NO_INPUT_ICON;
        }
    }

    // set the input name
    if (inputSelectionButton) {
        QFontMetrics fontMetrics = inputSelectionButton->fontMetrics();
        int elideWidth = inputSelectionButton->width() - inputSelectionButton->iconSize().width();
        QString elidedName = fontMetrics.elidedText(channelName, Qt::ElideRight, elideWidth);
        inputSelectionButton->setText(elidedName);
    }

    // set the icon
    if (inputTypeIconLabel)
        this->inputTypeIconLabel->setStyleSheet("background-image: url(" + iconFile + ");");

    setMidiPeakMeterVisibility(inputNode->isMidi());

    updateGeometry();

    update();
}

void LocalTrackViewStandalone::setMidiPeakMeterVisibility(bool visible)
{
    if (visible) {
        // midi activity meter is inserted between the 2 audio meters
        metersLayout->insertWidget(1, midiPeakMeter);
    } else {
        metersLayout->removeWidget(midiPeakMeter);
    }
    update();
}

void LocalTrackViewStandalone::setToMono(QAction *action)
{
    int selectedInputIndexInAudioDevice = action->data().toInt();
    controller->setInputTrackToMono(getTrackID(), selectedInputIndexInAudioDevice);
    setMidiPeakMeterVisibility(false);
    midiToolsButton->setVisible(false);
}

void LocalTrackViewStandalone::setToStereo(QAction *action)
{
    int firstInputIndexInAudioDevice = action->data().toInt();
    controller->setInputTrackToStereo(getTrackID(), firstInputIndexInAudioDevice);
    setMidiPeakMeterVisibility(false);
    midiToolsButton->setVisible(false);
}

void LocalTrackViewStandalone::setToMidi(QAction *action)
{
    QString indexes = action->data().toString();
    if (!indexes.contains(":"))
        return;
    QStringList indexesParts = indexes.split(":");
    if (indexesParts.size() != 2)
        return;
    QString midiChannelString = indexesParts.at(1);
    QString midiDeviceString = indexesParts.at(0);
    int midiChannel = midiChannelString.toInt();
    int midiDeviceIndex = midiDeviceString.toInt();

    controller->setInputTrackToMIDI(getTrackID(), midiDeviceIndex, midiChannel);

    midiToolsButton->setVisible(true);
}
