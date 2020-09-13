#include "LocalTrackGroupView.h"
#include "LocalTrackView.h"
#include "Highligther.h"
#include "MainWindow.h"
#include "log/Logging.h"
#include "MainController.h"
#include "widgets/BlinkableButton.h"
#include "IconFactory.h"
#include "widgets/InstrumentsMenu.h"
#include "gui/GuiUtils.h"
#include "audio/core/LocalInputNode.h"
#include "ninjam/client/Types.h"

#include <QInputDialog>
#include <QToolTip>
#include <QDateTime>
#include <QBoxLayout>

LocalTrackGroupView::LocalTrackGroupView(int channelIndex, MainWindow *mainWindow) :
    TrackGroupView(mainWindow),
    index(channelIndex),
    mainWindow(mainWindow),
    peakMeterOnly(false),
    videoChannel(false),
    preparingToTransmit(false),
    usingSmallSpacingInLayouts(false)
{
    instrumentsButton = createInstrumentsButton();
    topPanelLayout->addWidget(instrumentsButton, 1, Qt::AlignCenter);

    connect(instrumentsButton, &InstrumentsButton::iconChanged, this, &LocalTrackGroupView::instrumentIconChanged);


    toolButton = createToolButton();
    voiceChatButton = createVoiceChatButton();

    toolVoiceChatLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    toolVoiceChatLayout->setContentsMargins(5, 5, 5, 5);
    toolVoiceChatLayout->setSpacing(5);
    toolVoiceChatLayout->addWidget(toolButton, 0, Qt::AlignTop | Qt::AlignCenter);
    toolVoiceChatLayout->addWidget(voiceChatButton, 0, Qt::AlignBottom | Qt::AlignRight);
    topPanelLayout->addLayout(toolVoiceChatLayout);

    xmitButton = createXmitButton();
    layout()->addWidget(xmitButton);


    connect(toolButton, &QPushButton::clicked, this, &LocalTrackGroupView::showMenu);

    connect(xmitButton, &QPushButton::toggled, this, &LocalTrackGroupView::toggleTransmitingStatus);

    connect(voiceChatButton, &QPushButton::toggled, this, &LocalTrackGroupView::toggleVoiceChatStatus);

    translateUi();
}

void LocalTrackGroupView::setAsVideoChannel()
{
    if (index < 1 || videoChannel)
        return; // first channel can't be a video channel

    videoChannel = true;

    auto instrumentIcon = static_cast<int>(videoChannel ? InstrumentIndex::Video : InstrumentIndex::JamTabaIcon);
    setInstrumentIcon(instrumentIcon);
    instrumentsButton->setStyleSheet(QString("margin-left: 0px"));
    instrumentsButton->blockSignals(true);
    instrumentsButton->setVisible(!peakMeterOnly);
    xmitButton->setChecked(false);
    xmitButton->setVisible(false);

    auto tracks = getTracks<LocalTrackView *>();
    for (auto track : tracks) {
        track->setVisible(false);
        track->getInputNode()->setToNoInput();
    }

    toolButton->setVisible(false);

    updateXmitButtonText();

    voiceChatButton->setVisible(false);
}

InstrumentsButton *LocalTrackGroupView::createInstrumentsButton()
{
    auto defaultIcon = IconFactory::getDefaultInstrumentIcon();
    auto icons = IconFactory::getInstrumentIcons();

    return new InstrumentsButton(defaultIcon, icons, this);
}

QString LocalTrackGroupView::getChannelGroupName() const
{
    return instrumentIndexToString(static_cast<InstrumentIndex>(getInstrumentIcon()));
}

void LocalTrackGroupView::translateUi()
{
    updateXmitButtonText();

    xmitButton->setToolTip(tr("Enable/disable your audio transmission for others"));
    xmitButton->setAccessibleDescription(toolButton->toolTip());

    voiceChatButton->setToolTip(tr("Send your audio 'almost' in real time using very low quality (useful for quick conversations only)"));
    voiceChatButton->setAccessibleDescription(voiceChatButton->toolTip());

    toolButton->setToolTip(tr("Add or remove channels..."));
    toolButton->setAccessibleDescription(toolButton->toolTip());
}

void LocalTrackGroupView::updateXmitButtonText()
{
    if (peakMeterOnly || videoChannel) {
        xmitButton->setText(""); // no text, just the up arrow icon
    }
    else{
        xmitButton->setText(isPreparingToTransmit() ? tr("Preparing") : tr("Transmit"));
    }
}

LocalTrackGroupView::~LocalTrackGroupView()
{
}

void LocalTrackGroupView::setPreparingStatus(bool preparing)
{
    this->preparingToTransmit = preparing;
    xmitButton->setEnabled(!preparing);
    if (!isShowingPeakMeterOnly())
        updateXmitButtonText();

    xmitButton->setProperty("preparing", QVariant(preparing));// change the button property to change stylesheet
    style()->unpolish(xmitButton); // force the updating in stylesheet for "preparing" state in QPushButton
    style()->polish(xmitButton);
}

void LocalTrackGroupView::toggleTransmitingStatus(bool checked)
{
    if (!preparingToTransmit) { // users can't change xmit when is preparing
        setUnlightStatus(!checked);
        mainWindow->setTransmitingStatus(getChannelIndex(), checked);
    }
}

void LocalTrackGroupView::toggleVoiceChatStatus(bool checked)
{
    mainWindow->setVoiceChatStatus(getChannelIndex(), checked);

    voiceChatButton->toggleBlink();
}

BlinkableButton *LocalTrackGroupView::createXmitButton()
{
    auto *button = new BlinkableButton();
    button->setObjectName(QStringLiteral("xmitButton"));
    button->setCheckable(true);
    button->setChecked(true);
    button->setIcon(QIcon(":/images/transmit.png"));
    return button;
}

BlinkableButton *LocalTrackGroupView::createVoiceChatButton()
{
    auto *button = new BlinkableButton();
    button->setObjectName(QStringLiteral("voiceChatButton"));
    button->setCheckable(true);
    button->setChecked(false);
    button->setIcon(QIcon(":/images/mic.png"));
    return button;
}

QPushButton *LocalTrackGroupView::createToolButton()
{
    QPushButton *toolButton = new QPushButton();
    toolButton->setObjectName(QStringLiteral("toolButton"));
    toolButton->setText(QStringLiteral(""));
    toolButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    return toolButton;
}

void LocalTrackGroupView::closePluginsWindows()
{
    auto trackViews = getTracks<LocalTrackView *>();
    for (auto trackView : trackViews)
        trackView->closeAllPlugins();
}

void LocalTrackGroupView::addChannel()
{
    mainWindow->addChannelsGroup(-1); // using default instrument icon
}

void LocalTrackGroupView::setInstrumentIcon(int instrumentIndex)
{
    instrumentsButton->setInstrumentIcon(instrumentIndex);
}

int LocalTrackGroupView::getInstrumentIcon() const
{
    return instrumentsButton->getSelectedIcon();
}

void LocalTrackGroupView::resetTracks()
{
    for (auto track : getTracks<LocalTrackView *>())
        track->reset();

    if (!xmitButton->isChecked())
        xmitButton->click(); // uncheck/reset the xmit button, the default is xmiting (button checked)

    if (voiceChatButton->isChecked())
        voiceChatButton->click(); // the default is NOT use voice chat
}

QMenu *LocalTrackGroupView::createPresetsDeletingSubMenu()
{
    QMenu *deleteMenu = new QMenu(tr("Delete preset"));
    deleteMenu->setIcon(QIcon(":/images/preset-delete.png"));

    // adding a menu action for each stored preset
    QStringList presetsNames = Configurator::getInstance()->getPresetFilesNames(false);
    for (const QString &name : presetsNames) {
        QString stripedName = getStripedPresetName(name);
        QAction *deleteAction = deleteMenu->addAction(stripedName);
        deleteAction->setData(name); // putting the preset name (including .json suffix) in the Action instance we can get this preset name inside slot 'loadPreset'
    }

    connect(deleteMenu, &QMenu::triggered, this, &LocalTrackGroupView::deletePreset);

    deleteMenu->setEnabled(!presetsNames.isEmpty());

    return deleteMenu;
}

QMenu *LocalTrackGroupView::createPresetsLoadingSubMenu()
{
    // LOAD - using a submenu to list stored presets
    QMenu *loadMenu = new QMenu(tr("Load preset"));
    loadMenu->setIcon(QIcon(":/images/preset-load.png"));

    // adding a menu action for each stored preset
    QStringList presetsNames = Configurator::getInstance()->getPresetFilesNames(false);
    for (const QString &name : presetsNames) {
        QString stripedName = getStripedPresetName(name);
        QAction *loadAction = loadMenu->addAction(stripedName);
        loadAction->setData(name); // putting the preset name (including .json suffix) in the Action instance we can get this preset name inside slot 'loadPreset'
    }

    connect(loadMenu, &QMenu::triggered, this, &LocalTrackGroupView::loadPreset);

    loadMenu->setEnabled(!presetsNames.isEmpty());

    return loadMenu;
}

void LocalTrackGroupView::createPresetsActions(QMenu &menu)
{
    menu.addMenu(createPresetsLoadingSubMenu()); // loading submenu

    // save preset
    QAction *addPresetActionSave = menu.addAction(QIcon(":/images/preset-save.png"), tr("Save preset"));
    connect(addPresetActionSave, &QAction::triggered, this, &LocalTrackGroupView::savePreset);

    menu.addMenu(createPresetsDeletingSubMenu()); // deleting submenu

    // RESET - in case of panic
    QAction *reset = menu.addAction(QIcon(":/images/gear.png"), tr("Reset Track Controls"));
    connect(reset, &QAction::triggered, this, &LocalTrackGroupView::resetLocalTracks);
}

void LocalTrackGroupView::createChannelsActions(QMenu &menu)
{
    QAction *addChannelAction = menu.addAction(QIcon(":/images/more.png"), tr("Add channel"));
    connect(addChannelAction, &QAction::triggered, this, &LocalTrackGroupView::addChannel);

    addChannelAction->setEnabled(mainWindow->getChannelGroupsCount() < 2); // at this moment users can't create more channels
    if (mainWindow->getChannelGroupsCount() > 1) {
        for (int i = 2; i <= mainWindow->getChannelGroupsCount(); ++i) {
            QString channelName = mainWindow->getChannelGroupName(i-1);
            QIcon icon(":/images/less.png");
            QString text = tr("Remove channel \"%1\"").arg(channelName);
            QAction *action = menu.addAction(icon, text);
            action->setData(i-1);  // use channel index as action data
            connect(action, &QAction::triggered, this, &LocalTrackGroupView::removeChannel);
            connect(action, &QAction::hovered, this, &LocalTrackGroupView::highlightHoveredChannel);
        }
    }
}

void LocalTrackGroupView::populateMenu(QMenu &menu)
{
    createPresetsActions(menu);
    menu.addSeparator();
    createChannelsActions(menu);
    menu.addSeparator();
}

void LocalTrackGroupView::showMenu()
{
    QMenu menu;
    populateMenu(menu); // populateMenu is overrided in Standalone to add subchannel actions
    menu.move(mapToGlobal(toolButton->pos() + QPoint(toolButton->width(), 0)));
    menu.exec();
}

void LocalTrackGroupView::addSubChannel()
{
    if (!trackViews.isEmpty()) {
        addTrackView(getChannelIndex());

        auto firstSubchannel = trackViews.first();
        auto lastSubchannel = trackViews.last();

        lastSubchannel->setTintColor(firstSubchannel->getTintColor());
    }
}

int LocalTrackGroupView::getSubchannelInternalIndex(uint subchannelTrackID) const
{
    for (int i = 0; i < trackViews.count(); ++i) {
        if (static_cast<uint>(trackViews.at(i)->getTrackID()) == subchannelTrackID)
            return i;
    }

    return -1;
}

LocalTrackView *LocalTrackGroupView::addTrackView(long trackID)
{
    if (trackViews.size() >= MAX_SUB_CHANNELS)
        return nullptr;

    LocalTrackView *newTrack = dynamic_cast<LocalTrackView *>(TrackGroupView::addTrackView(trackID));

    bool enableLooperButton = mainWindow->getMainController()->isPlayingInNinjamRoom();
    newTrack->enableLopperButton(enableLooperButton);
    connect(newTrack, &LocalTrackView::openLooperEditor, mainWindow, &MainWindow::openLooperWindow);

    if (newTrack)
        emit trackAdded();

    return newTrack;
}

LocalTrackView *LocalTrackGroupView::createTrackView(long trackID)
{
    return new LocalTrackView(mainWindow->getMainController(), trackID);
}

void LocalTrackGroupView::setToWide()
{
    if (trackViews.count() <= 1) { // don't allow 2 wide subchannels
        for (BaseTrackView *trackView : this->trackViews) {
            trackView->setToWide();
        }
    }

    updateGeometry();
}

void LocalTrackGroupView::setToNarrow()
{
    for (BaseTrackView *trackView : this->trackViews) {
        trackView->setToNarrow();
    }

    updateGeometry();
}

void LocalTrackGroupView::highlightHoveredChannel()
{
    int channelGroupIndex = 1; // just the second channel can be highlighted at moment
    if (channelGroupIndex < mainWindow->getChannelGroupsCount())
        mainWindow->highlightChannelGroup(channelGroupIndex);
}

void LocalTrackGroupView::highlightHoveredSubchannel()
{
    int subChannelIndex = 1; // just the second subchannel can be highlighted at moment
    if (subChannelIndex < trackViews.size())
        Highligther::getInstance()->highlight(trackViews.at(subChannelIndex));
}

void LocalTrackGroupView::removeSubchannel()
{
    if (trackViews.size() > 1) { // can't remove the default/first subchannel
        removeTrackView(1); // always remove the second channel
        emit trackRemoved();
    }
}

void LocalTrackGroupView::detachMainControllerInSubchannels()
{
    for (LocalTrackView *view : getTracks<LocalTrackView *>())
        view->detachMainController();
}

void LocalTrackGroupView::removeChannel()
{
    mainWindow->removeChannelsGroup(mainWindow->getChannelGroupsCount()-1);
}

// PRESETS
void LocalTrackGroupView::loadPreset(QAction *action)
{
    auto mainController = mainWindow->getMainController();
    QString presetFileName = action->data().toString();
    auto preset = mainController->loadPreset(presetFileName);
    if (preset.isValid()) {
        mainWindow->loadPreset(preset);

        // send the new channels to other musicians
        mainController->sendNewChannelsNames(mainWindow->getChannelsMetadata());
    }
}

void LocalTrackGroupView::savePreset()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Save the preset ..."),
                                         tr("Preset name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty()) {
        text.append(".json");
        mainWindow->getMainController()->savePreset(mainWindow->getInputsSettings(), text);
    }
}

void LocalTrackGroupView::resetLocalTracks()
{
    mainWindow->resetLocalChannels(); // reset all local channels and subchannels
}

QString LocalTrackGroupView::getStripedPresetName(const QString &presetName)
{
    if (presetName.endsWith(".json"))
        return presetName.left(presetName.size() - 5); // remove '.json' 5 letters

    return presetName;
}

void LocalTrackGroupView::deletePreset(QAction *action)
{
    QString presetName = action->data().toString();
    QString stripedName = getStripedPresetName(presetName);
    QMessageBox::StandardButton reply;
    QString messageBoxTitle(tr("Deleting preset ..."));
    QString messageBoxText(tr("You want to delete the preset '%1'").arg(stripedName));
    reply = QMessageBox::question(this, messageBoxTitle, messageBoxText, QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
        mainWindow->getMainController()->deletePreset(presetName);
}

// +++++++++++++++++++++++++++++

// fixing #962

QSize LocalTrackGroupView::sizeHint() const
{
    if (peakMeterOnly || videoChannel)
        return QFrame::sizeHint();

    return TrackGroupView::sizeHint();
}

void LocalTrackGroupView::setPeakMeterMode(bool peakMeterOnly)
{
    if (this->peakMeterOnly != peakMeterOnly) {
        this->peakMeterOnly = peakMeterOnly;

        toolButton->setVisible(!peakMeterOnly);
        instrumentsButton->setVisible(!peakMeterOnly);

        for (auto view : getTracks<LocalTrackView *>()) {
            view->setPeakMetersOnlyMode(peakMeterOnly);
        }

        updateXmitButtonText();
        updateGeometry();
    }
}

void LocalTrackGroupView::togglePeakMeterOnlyMode()
{
    setPeakMeterMode(!this->peakMeterOnly);
}

