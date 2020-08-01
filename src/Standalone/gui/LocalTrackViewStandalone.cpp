#include "LocalTrackViewStandalone.h"
#include "audio/core/LocalInputNode.h"
#include "audio/core/Plugins.h"
#include "FxPanel.h"
#include "FxPanelItem.h"
#include "gui/widgets/BoostSpinBox.h"
#include "gui/IconFactory.h"
#include "log/Logging.h"

#include <QGridLayout>
#include <QStyle>
#include <QSlider>
#include <QPainter>
#include <QDesktopWidget>

LocalTrackViewStandalone::LocalTrackViewStandalone(controller::MainControllerStandalone *mainController, int channelIndex) :
    LocalTrackView(mainController, channelIndex),
    controller(mainController),
    midiToolsDialog(nullptr),
    midiRoutingArrowColor(QColor(255, 0, 0, 100))
{
    fxPanel = createFxPanel();

    QMargins contentMargins = mainLayout->contentsMargins();
    contentMargins.setTop(8); // add a top margin to draw midi routing arrow
    mainLayout->setContentsMargins(contentMargins);

    mainLayout->addWidget(fxPanel, mainLayout->rowCount(), 0, 1, 2);

    // create input panel in the bottom
    this->inputPanel = createInputPanel();

    mainLayout->addWidget(inputPanel, mainLayout->rowCount(), 0, 1, 2);

    midiToolsButton = createMidiToolsButton();
    mainLayout->addWidget(midiToolsButton, mainLayout->rowCount(), 0, 1, 2);

    this->inputTypeIconLabel = createInputTypeIconLabel(this);
    mainLayout->addWidget(inputTypeIconLabel, mainLayout->rowCount(), 0, 1, 2);
    mainLayout->setAlignment(this->inputTypeIconLabel, Qt::AlignCenter);

    // create the peak meter to show midiactivity
    midiPeakMeter = new MidiActivityMeter(this);
    midiPeakMeter->setObjectName(QStringLiteral("midiPeakMeter"));
    midiPeakMeter->setSolidColor(QColor(180, 0, 0));
    midiPeakMeter->setDecayTime(1000);// 1000 ms
    midiPeakMeter->setAccessibleDescription("This is the midi activity meter");
    secondaryChildsLayout->insertWidget(0, midiPeakMeter, 1, Qt::AlignHCenter);
    secondaryChildsLayout->insertSpacerItem(1, new QSpacerItem(1, 6, QSizePolicy::Maximum, QSizePolicy::MinimumExpanding)); // keep the buttons in bottom when midiActivityMeter is not visible

    inputSelectionButton->installEventFilter(this);

    auto inputNode = getInputNode();
    connect(inputNode, &audio::LocalInputNode::midiNoteLearned, this, &LocalTrackViewStandalone::useLearnedMidiNote);

    translateUI();
}

void LocalTrackViewStandalone::setTintColor(const QColor &color)
{
    LocalTrackView::setTintColor(color);

    updateInputIcon();
}

void LocalTrackViewStandalone::setToMidi()
{
    int midiChannel = -1;
    int midiDeviceIndex = 0;

    controller->setInputTrackToMIDI(getTrackID(), midiDeviceIndex, midiChannel);

    emit trackInputChanged();
}

bool LocalTrackViewStandalone::isNoInput() const
{
    return inputNode->isNoInput();
}

bool LocalTrackViewStandalone::isMidi() const
{
    return inputNode->isMidi();
}

void LocalTrackViewStandalone::paintRoutingMidiArrow(int topMargin, int arrowSize, bool drawMidiWord)
{
    QPainter painter(this);
    painter.setPen(QPen(midiRoutingArrowColor));

    const int leftMargin = 1;

    int metersCenter = midiPeakMeter->x() + midiPeakMeter->width()/2.0;
    int x1 = metersCenter - 2;
    int y = midiPeakMeter->y() - 2;
    int x2 = metersCenter + 2;

    // draw the vertical line
    int x = x1 + (x2 - x1)/2.0;
    painter.drawLine(x, y+1, x, topMargin);

    // draw the horizontal line pointing to left subchannel
    y = topMargin;
    painter.drawLine(x-1, y, leftMargin + arrowSize, y);

    if (drawMidiWord) { // the MIDI word is not drawed when local tracks are collapsed
        // draw MIDI word
        QString text("MIDI");
        static const QFont smallFont(font().family(), 5);
        setFont(smallFont);
        int textX = midiPeakMeter->x() - fontMetrics().width(text) - 3;
        painter.drawText(textX, y + fontMetrics().height() + 1, text);
    }

    // draw arrow in left side
    QPainterPath arrow(QPointF(leftMargin, y+1));
    arrow.lineTo(QPointF(leftMargin + arrowSize, y - arrowSize));
    arrow.lineTo(QPointF(leftMargin + arrowSize, y + arrowSize));
    arrow.closeSubpath();

    painter.setBrush(midiRoutingArrowColor);
    painter.setPen(Qt::NoPen);
    painter.drawPath(arrow);
}

void LocalTrackViewStandalone::paintReceivingRoutedMidiIndicator(int topMargin)
{
    QPainter painter(this);

    painter.setPen(QPen(midiRoutingArrowColor));

    const int rightMargin = 2;

    int metersCenter = midiPeakMeter->x() + midiPeakMeter->width()/2.0;
    if (peakMetersOnly)
        metersCenter = levelSlider->x() + levelSlider->width()/2.0;

    int x1 = metersCenter - 2;
    int y = midiPeakMeter->y() - 2;
    int x2 = metersCenter + 2;

    // draw the vertical line
    int x = x1 + (x2 - x1)/2.0;
    painter.drawLine(x, y, x, topMargin);

    // draw the horizontal line pointing to right subchannel
    y = topMargin;
    x2 = width() - rightMargin;
    painter.drawLine(x+1, y, x2, y);
}

void LocalTrackViewStandalone::paintEvent(QPaintEvent *ev)
{
    LocalTrackView::paintEvent(ev);

    const int topMargin = levelSlider->y() - 6;
    static const int arrowSize = 4;

    if (inputNode->isRoutingMidiInput()) {
        bool drawMidiWord = !isShowingPeakMetersOnly();
        paintRoutingMidiArrow(topMargin, arrowSize, drawMidiWord);
    }
    else {
        if (inputNode->isReceivingRoutedMidiInput()) {
            paintReceivingRoutedMidiIndicator(topMargin);
        }
    }
}

void LocalTrackViewStandalone::translateUI()
{
    LocalTrackView::translateUI();

    inputSelectionButton->setToolTip(tr("Choose input channels ..."));

    midiToolsButton->setText(tr("Tools"));

    refreshInputSelectionName();
}

void LocalTrackViewStandalone::useLearnedMidiNote(quint8 midiNote)
{
    if (midiToolsDialog && midiToolsDialog->isVisible()) {
        QString midiNoteName = getMidiNoteText(midiNote);
        midiToolsDialog->setLearnedMidiNote(midiNoteName);
    }
}

void LocalTrackViewStandalone::updateGuiElements()
{
    LocalTrackView::updateGuiElements();

    if (inputNode && inputNode->hasMidiActivity()) {
        quint8 midiActivityValue = inputNode->getMidiActivityValue();
        midiPeakMeter->setActivityValue(midiActivityValue/127.0);
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

    setMidiRouting(false);
  
    refreshInputSelectionName();
}

QLabel *LocalTrackViewStandalone::createInputTypeIconLabel(QWidget *parent)
{
    QLabel *label = new QLabel(parent);
    label->setObjectName(QStringLiteral("inputSelectionIcon"));
    label->setTextFormat(Qt::RichText);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    return label;
}

QPushButton *LocalTrackViewStandalone::createInputSelectionButton(QWidget *parent)
{
    QPushButton *fakeComboButton = new QPushButton(parent);
    fakeComboButton->setObjectName(QStringLiteral("inputSelectionButton"));
    fakeComboButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QObject::connect(fakeComboButton, SIGNAL(clicked()), this, SLOT(showInputSelectionMenu()));

    return fakeComboButton;
}

QWidget *LocalTrackViewStandalone::createInputPanel()
{
    QWidget *inputPanel = new QWidget(this);
    inputPanel->setObjectName(QStringLiteral("inputPanel"));
    inputPanel->setLayout(new QHBoxLayout(inputPanel));
    inputPanel->layout()->setContentsMargins(0, 0, 0, 0);
    inputPanel->layout()->setSpacing(0);
    inputPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    this->inputSelectionButton = createInputSelectionButton(inputPanel);
    inputPanel->layout()->addWidget(inputSelectionButton);// button in right

    return inputPanel;
}

QPushButton *LocalTrackViewStandalone::createMidiToolsButton()
{
    QPushButton * button = new QPushButton();
    button->setObjectName(QStringLiteral("midiToolsButton"));
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    button->setVisible(false);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(openMidiToolsDialog()));
    return button;
}

void LocalTrackViewStandalone::setAudioRelatedControlsStatus(bool enableControls)
{

    QWidget *controls[] = {
        boostSpinBox,
        soloButton,
        muteButton,
        buttonStereoInversion,
        dynamic_cast<QWidget *>(panSlider),
        dynamic_cast<QWidget *>(levelSlider)
    };

    for (QWidget *control : controls) {
        control->setEnabled(enableControls);
    }

    fxPanel->setEnabled(enableControls);

}

void LocalTrackViewStandalone::setMidiRouting(bool routingMidiToFirstSubchannel)
{
    inputNode->setRoutingMidiInput(routingMidiToFirstSubchannel);

    bool enableAudioControls = !inputNode->isRoutingMidiInput();
    setAudioRelatedControlsStatus(enableAudioControls);
    enableLopperButton(enableAudioControls && mainController->isPlayingInNinjamRoom());
    update();
}

bool LocalTrackViewStandalone::isFirstSubchannel() const
{
    auto firstSubchannel = mainController->getInputTrackInGroup(inputNode->getChanneGroupIndex(), 0);

    return firstSubchannel == this->inputNode;
}

void LocalTrackViewStandalone::openMidiToolsDialog()
{
    if (!midiToolsDialog) {
        qCDebug(jtGUI) << "Creating a new MidiToolsDialog!";
        auto inputNode = getInputNode();
        QString higherNote = getMidiNoteText(inputNode->getMidiHigherNote());
        QString lowerNote = getMidiNoteText(inputNode->getMidiLowerNote());
        qint8 transpose = inputNode->getTranspose();
        bool routingMidiInput = inputNode->isRoutingMidiInput();

        midiToolsDialog = new MidiToolsDialog(lowerNote, higherNote, transpose, routingMidiInput);
        if (isFirstSubchannel())
            midiToolsDialog->hideMidiRoutingControls(); // midi routing is available only in second subchannel

        connect(midiToolsDialog, &MidiToolsDialog::dialogClosed, this, &LocalTrackViewStandalone::onMidiToolsDialogClosed);
        connect(midiToolsDialog, &MidiToolsDialog::lowerNoteChanged, this, &LocalTrackViewStandalone::setMidiLowerNote);
        connect(midiToolsDialog, &MidiToolsDialog::higherNoteChanged, this, &LocalTrackViewStandalone::setMidiHigherNote);
        connect(midiToolsDialog, &MidiToolsDialog::transposeChanged, this, &LocalTrackViewStandalone::setTranspose);
        connect(midiToolsDialog, &MidiToolsDialog::learnMidiNoteClicked, this, &LocalTrackViewStandalone::toggleMidiNoteLearn);
        connect(midiToolsDialog, &MidiToolsDialog::midiRoutingCheckBoxClicked, this, &LocalTrackViewStandalone::setMidiRouting);
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

void LocalTrackViewStandalone::toggleMidiNoteLearn(bool buttonClicked)
{
    if (buttonClicked)
        startMidiNoteLearn();
    else
        stopMidiNoteLearn();
}

void LocalTrackViewStandalone::startMidiNoteLearn()
{
    auto inputNode = getInputNode();
    if (inputNode) {
        inputNode->startMidiNoteLearn();
    }
}

void LocalTrackViewStandalone::stopMidiNoteLearn()
{
    auto inputNode = getInputNode();
    if (inputNode) {
        inputNode->stopMidiNoteLearn();
    }
}

void LocalTrackViewStandalone::setTranspose(qint8 transposeValue)
{
     auto inputNode = getInputNode();
     if (inputNode) {
         inputNode->setTranspose(transposeValue);
     }
}

void LocalTrackViewStandalone::setMidiHigherNote(const QString &higherNote)
{
    auto inputNode = getInputNode();
    if (inputNode) {
        quint8 noteNumber = getMidiNoteNumber(higherNote);
        inputNode->setMidiHigherNote(noteNumber);
    }
}

void LocalTrackViewStandalone::setMidiLowerNote(const QString &lowerNote)
{
    auto inputNode = getInputNode();
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
        case 0: noteName = QStringLiteral("C"); break;
        case 1: noteName = QStringLiteral("C#"); break;
        case 2: noteName = QStringLiteral("D"); break;
        case 3: noteName = QStringLiteral("D#"); break;
        case 4: noteName = QStringLiteral("E"); break;
        case 5: noteName = QStringLiteral("F"); break;
        case 6: noteName = QStringLiteral("F#"); break;
        case 7: noteName = QStringLiteral("G"); break;
        case 8: noteName = QStringLiteral("G#"); break;
        case 9: noteName = QStringLiteral("A"); break;
        case 10: noteName = QStringLiteral("A#"); break;
        case 11: noteName = QStringLiteral("B"); break;
    }
    return noteName + QString::number(octave);
}

quint8 LocalTrackViewStandalone::getMidiNoteNumber(const QString &midiNote) const
{
    // midi note numbers: http://www.midimountain.com/midi/midi_note_numbers.html

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
     int octave = midiNote.at( (midiNote.length() > 2) ? 2 : 1).toLatin1() - 48; // ascii to int

     noteNumber += (octave * 12);

     if (midiNote.contains('b'))
         noteNumber--;
     if (midiNote.contains('#'))
         noteNumber++;

     return noteNumber;
}

void LocalTrackViewStandalone::onMidiToolsDialogClosed()
{
    midiToolsDialog = nullptr;
    stopMidiNoteLearn();
    qCDebug(jtGUI) << "MidiToolsDialog pointer cleared!";
}

void LocalTrackViewStandalone::addPlugin(audio::Plugin *plugin, quint32 slotIndex, bool bypassed)
{
    if (fxPanel) {
        plugin->setBypass(bypassed);
        fxPanel->addPlugin(plugin, slotIndex);
        refreshInputSelectionName(); // refresh input type combo box, if the added plugins is a virtual instrument Jamtaba will try auto change the input type to midi
        update();
    }
}

qint32 LocalTrackViewStandalone::getPluginFreeSlotIndex() const
{
    return fxPanel->getPluginFreeSlotIndex();
}

QList<const audio::Plugin *> LocalTrackViewStandalone::getInsertedPlugins() const
{
    QList<const audio::Plugin *> plugins;
    if (fxPanel) { // can be nullptr in vst plugin
        for (auto item : fxPanel->getItems()) {
            if (item->containPlugin())
                plugins.append(item->getAudioPlugin());
        }
    }
    return plugins;
}

FxPanel *LocalTrackViewStandalone::createFxPanel()
{
    return new FxPanel(this, controller);
}

void LocalTrackViewStandalone::setActivatedStatus(bool unlighted)
{
    LocalTrackView::setActivatedStatus(unlighted);

    if (fxPanel) {
        style()->unpolish(fxPanel);
        style()->polish(fxPanel);

        QList<FxPanelItem *> items = fxPanel->getItems();
        for (FxPanelItem *item : items) {
            style()->unpolish(item);
            style()->polish(item);
        }
    }

    if (inputPanel) {
        style()->unpolish(inputPanel);
        style()->polish(inputPanel);
    }
}

void LocalTrackViewStandalone::setPeakMetersOnlyMode(bool peakMetersOnly)
{
    LocalTrackView::setPeakMetersOnlyMode(peakMetersOnly);

    Q_ASSERT(fxPanel);
    fxPanel->setVisible(!peakMetersOnly);
    inputPanel->setVisible(!peakMetersOnly);

    Q_ASSERT(inputTypeIconLabel);
    inputTypeIconLabel->setVisible(canShowInputTypeIcon());

    Q_ASSERT(midiToolsButton);
    midiToolsButton->setVisible(canShowMidiToolsButton());
}

void LocalTrackViewStandalone::setupMetersLayout()
{
    LocalTrackView::setupMetersLayout();
    if (midiPeakMeter) {
        midiPeakMeter->setVisible(inputNode->isMidi() || inputNode->isRoutingMidiInput() || inputNode->isReceivingRoutedMidiInput());
    }
}

QMenu *LocalTrackViewStandalone::createMonoInputsMenu(QMenu *parent)
{
    auto monoInputsMenu = new QMenu(tr("Mono"), parent);
    monoInputsMenu->setIcon(QIcon(IconFactory::createMonoInputIcon(tintColor)));
    auto audioDriver = controller->getAudioDriver();
    int globalInputs = audioDriver->getInputsCount();
    QString deviceName(audioDriver->getAudioInputDeviceName(audioDriver->getAudioInputDeviceIndex()));

    for (int i = 0; i < globalInputs; ++i) {
        int index = audioDriver->getFirstSelectedInput() + i;
        QString channelName = QString(audioDriver->getInputChannelName(index)).trimmed();
        if (channelName.isNull() || channelName.isEmpty())
            channelName = QString::number(index+1)  + " "+ deviceName;
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
    auto icon = QIcon(IconFactory::createNoInputIcon(tintColor));
    QAction *noInputAction = menu.addAction(icon, getNoInputText());
    QObject::connect(noInputAction, SIGNAL(triggered()), this, SLOT(setToNoInput()));

    menu.move(mapToGlobal(inputSelectionButton->parentWidget()->pos()));
    menu.exec();
}

QString LocalTrackViewStandalone::getNoInputText()
{
    return tr("no input");
}

QMenu *LocalTrackViewStandalone::createStereoInputsMenu(QMenu *parent)
{
    auto stereoInputsMenu = new QMenu(tr("Stereo"), parent);
    stereoInputsMenu->setIcon(QIcon(IconFactory::createStereoInputIcon(tintColor)));
    auto audioDriver = controller->getAudioDriver();
    int globalInputs = audioDriver->getInputsCount();
    QString deviceName(audioDriver->getAudioInputDeviceName(audioDriver->getAudioInputDeviceIndex()));
    for (int i = 0; i < globalInputs; i += 2) {
        if (i + 1 < globalInputs) { // we can make a channel pair using (i) and (i+1)?
            QString firstName = getInputChannelNameOnly(i);
            QString indexes = QString::number(i+1) + " + " + QString::number(i+2);
            QString inputName = firstName + " [" + indexes +  "]  (" + deviceName + ")";
            QAction *action = stereoInputsMenu->addAction(inputName);
            action->setData(i); // use the first input pair index as action data
            action->setIcon(stereoInputsMenu->icon());
        }
    }
    stereoInputsMenu->setEnabled(globalInputs/2 >= 1); // at least one pair
    if (!stereoInputsMenu->isEnabled()) {
        QString msg = tr("%1  (not enough available inputs to make stereo)").arg(stereoInputsMenu->title());
        stereoInputsMenu->setTitle(msg);
    }
    QObject::connect(stereoInputsMenu, SIGNAL(triggered(QAction *)), this, SLOT(setToStereo(QAction *)));
    return stereoInputsMenu;
}

QString LocalTrackViewStandalone::getInputChannelNameOnly(int inputIndex)
{
    auto audioDriver = controller->getAudioDriver();
    QString fullName(audioDriver->getInputChannelName(inputIndex));
    if (fullName.isEmpty()) // mac return empy channel names if user don't rename the channels
        fullName = audioDriver->getAudioInputDeviceName();
    int spaceIndex = fullName.lastIndexOf(" ");
    if (spaceIndex > 0)
        return fullName.left(spaceIndex);
    return fullName;
}

QMenu *LocalTrackViewStandalone::createMidiInputsMenu(QMenu *parent)
{
    auto midiInputsMenu = new QMenu(tr("MIDI"), parent);
    midiInputsMenu->setIcon(QIcon(IconFactory::createMidiIcon(tintColor)));
    auto midiDriver = controller->getMidiDriver();
    int totalMidiDevices = midiDriver->getMaxInputDevices();
    int globallyEnabledMidiDevices = 0;
    for (int d = 0; d < totalMidiDevices; ++d) {
        if (midiDriver->inputDeviceIsGloballyEnabled(d)) {
            globallyEnabledMidiDevices++;
            auto midiChannelsMenu = new QMenu(midiInputsMenu);
            auto actionGroup = new QActionGroup(midiChannelsMenu);

            auto allChannelsAction = midiChannelsMenu->addAction(tr("All channels"));
            allChannelsAction->setData(QString(QString::number(d) + ":" + QString::number(-1))); // use -1 to all channels
            allChannelsAction->setActionGroup(actionGroup);
            allChannelsAction->setCheckable(true);
            auto inputNode = getInputNode();
            allChannelsAction->setChecked(
                inputNode->isMidi() && inputNode->getMidiDeviceIndex() == d
                && inputNode->isReceivingAllMidiChannels());

            midiChannelsMenu->addSeparator();
            for (int c = 0; c < 16; ++c) {
                auto a = midiChannelsMenu->addAction(tr("Channel %1").arg(QString::number(c+1)));
                a->setData(QString(QString::number(d) + ":" + QString::number(c))); // use device:channel_index as data
                a->setActionGroup(actionGroup);
                a->setCheckable(true);
                a->setChecked(
                    inputNode->isMidi() && inputNode->getMidiChannelIndex() == c
                    && inputNode->getMidiDeviceIndex() == d);
            }

            QString deviceName = controller->getMidiDriver()->getInputDeviceName(d);
            auto action = midiInputsMenu->addAction(deviceName);
            action->setMenu(midiChannelsMenu);
            action->setData(d); // using midi device index as action data
            action->setIcon(midiInputsMenu->icon());
            QObject::connect(midiChannelsMenu, SIGNAL(triggered(QAction *)), this,
                             SLOT(setToMidi(QAction *)));
        }
    }
    midiInputsMenu->setEnabled(globallyEnabledMidiDevices > 0);
    if (!midiInputsMenu->isEnabled()) {
        midiInputsMenu->setTitle(tr("%1  (no MIDI devices detected or enabled in 'Preferences' menu')").arg(midiInputsMenu->title()));
    }
    return midiInputsMenu;
}

void LocalTrackViewStandalone::setToNoInput()
{
    if (inputNode) {

        if (inputNode->isRoutingMidiInput())
            setMidiRouting(false);

        inputNode->setToNoInput();

        refreshInputSelectionName();

        emit trackInputChanged();
    }
}

QPixmap LocalTrackViewStandalone::getInputTypePixmap()
{
    if (inputNode->isAudio()) { // using audio as input method
        if (inputNode->isStereo())
            return IconFactory::createStereoInputIcon(tintColor);
        else if (inputNode->isMono())
            return IconFactory::createMonoInputIcon(tintColor);
        else
            return IconFactory::createNoInputIcon(tintColor); // range is empty = no audio input
    }

    if (inputNode->isMidi()) {
        if (canUseMidiDeviceIndex(inputNode->getMidiDeviceIndex()))
            return IconFactory::createMidiIcon(tintColor);
    }

    return IconFactory::createNoInputIcon(tintColor);
}

bool LocalTrackViewStandalone::canUseMidiDeviceIndex(int midiDeviceIndex) const
{
    auto midiDriver = controller->getMidiDriver();
    if (midiDeviceIndex < midiDriver->getMaxInputDevices() && midiDriver->inputDeviceIsGloballyEnabled(midiDeviceIndex))
        return true;

    return false;
}

void LocalTrackViewStandalone::updateInputIcon()
{
    // set the icon
    Q_ASSERT(inputTypeIconLabel);
    inputTypeIconLabel->setPixmap(getInputTypePixmap());
}

void LocalTrackViewStandalone::refreshInputSelectionName()
{
    auto inputTrack = controller->getInputTrack(getTrackID());

    if (inputTrack->isMidi() && !canUseMidiDeviceIndex(inputTrack->getMidiDeviceIndex()))
        inputTrack->setToNoInput();

    updateInputText();
    updateInputIcon();

    setMidiPeakMeterVisibility(inputNode->isMidi() || inputNode->isRoutingMidiInput() || inputNode->isReceivingRoutedMidiInput());

    midiToolsButton->setVisible( canShowMidiToolsButton() );
    inputTypeIconLabel->setVisible(canShowInputTypeIcon());

    buttonStereoInversion->setEnabled(!inputNode->isMono() && !inputNode->isRoutingMidiInput()); //stereo, midi or no-input can be stereo inverted. Sometimes we are using 'no-input' but using some VSTi generating loops, for example. These VSTi will generate stereo output and can be inverted.

    updateStyleSheet();
    updateGeometry();
    update();
}

QString LocalTrackViewStandalone::getAudioInputText()
{
    auto inputRange = inputNode->getAudioInputRange();
    if (inputNode->isStereo()) {
        int firstInputIndex = inputRange.getFirstChannel() + controller->getAudioDriver()->getFirstSelectedInput();
        QString indexes = "(" + QString::number(firstInputIndex+1) + "+" + QString::number(firstInputIndex+2) + ") ";
        return indexes + getInputChannelNameOnly(firstInputIndex);
    }

    if (inputNode->isMono()) {
        auto audioDriver = controller->getAudioDriver();
        int index = inputRange.getFirstChannel() + controller->getAudioDriver()->getFirstSelectedInput();
        QString name = QString(audioDriver->getInputChannelName(index));
        QString inputType = QString(QString::number(index+1) + " - ");
        if (!name.isNull() && !name.isEmpty())
            inputType += name;
        else
            inputType += QString(audioDriver->getAudioInputDeviceName());

        return inputType;
    }

    // range is empty = no audio input
    return getNoInputText();
}

QString LocalTrackViewStandalone::getMidiInputText()
{
    auto midiDriver = controller->getMidiDriver();
    int selectedDeviceIndex = inputNode->getMidiDeviceIndex();
    if (selectedDeviceIndex < midiDriver->getMaxInputDevices() && midiDriver->inputDeviceIsGloballyEnabled(selectedDeviceIndex)) {
        return midiDriver->getInputDeviceName(selectedDeviceIndex);
    } // midi device index invalid

    return getNoInputText();
}

QString LocalTrackViewStandalone::getInputText()
{
    if (inputNode->isAudio()) { // using audio as input method
        return getAudioInputText();
    }

    if (inputNode->isMidi()) {
        return getMidiInputText();
    }

    return getNoInputText();
}

void LocalTrackViewStandalone::updateInputText()
{
    Q_ASSERT(inputSelectionButton);
    QFontMetrics fontMetrics = inputSelectionButton->fontMetrics();
    int inputSelectionWidth = inputSelectionButton->width();
    int elideWidth = inputSelectionWidth - inputSelectionButton->iconSize().width();
    QString inputType = getInputText();
    QString elidedName = fontMetrics.elidedText(inputType, Qt::ElideRight, elideWidth);
    inputSelectionButton->setText(elidedName);
}

bool LocalTrackViewStandalone::canShowInputTypeIcon() const
{
    return !inputNode->isMidi() && !isShowingPeakMetersOnly();
}

bool LocalTrackViewStandalone::canShowMidiToolsButton() const
{
    return inputNode->isMidi() && !isShowingPeakMetersOnly();
}

void LocalTrackViewStandalone::setMidiPeakMeterVisibility(bool visible)
{
    if (midiPeakMeter)
        midiPeakMeter->setVisible(visible);
}

void LocalTrackViewStandalone::setToMono(QAction *action)
{
    if (inputNode->isRoutingMidiInput())
        setMidiRouting(false);

    int selectedInputIndexInAudioDevice = action->data().toInt();
    controller->setInputTrackToMono(getTrackID(), selectedInputIndexInAudioDevice);
    setMidiPeakMeterVisibility(false);
    levelSlider->setStereo(false);

    emit trackInputChanged();
}

void LocalTrackViewStandalone::setToStereo(QAction *action)
{
    if (inputNode->isRoutingMidiInput())
        setMidiRouting(false);

    int firstInputIndexInAudioDevice = action->data().toInt();
    controller->setInputTrackToStereo(getTrackID(), firstInputIndexInAudioDevice);
    setMidiPeakMeterVisibility(false);

    levelSlider->setStereo(true);

    emit trackInputChanged();
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

    levelSlider->setStereo(true);

    emit trackInputChanged();
}

bool LocalTrackViewStandalone::eventFilter(QObject *target, QEvent *event)
{
    if (target == inputSelectionButton) {
        if (event->type() == QEvent::Resize) {
            auto resizeEvent = static_cast<QResizeEvent *>(event);
            if (resizeEvent->size().width() > resizeEvent->oldSize().width()) { // is growing?
                updateInputText();
                return true;
            }
        }
    }
    return LocalTrackView::eventFilter(target, event);
}
