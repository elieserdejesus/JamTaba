#include "LocalTrackGroupView.h"
#include "LocalTrackView.h"
#include "Highligther.h"
#include "MainWindow.h"
#include "log/Logging.h"
#include <QInputDialog>
#include "MainController.h"

LocalTrackGroupView::LocalTrackGroupView(int channelIndex, MainWindow *mainFrame) :
    index(channelIndex),
    mainFrame(mainFrame),
    peakMeterOnly(false),
    preparingToTransmit(false)
{
    toolButton = createToolButton();
    topPanel->layout()->addWidget(toolButton);

    xmitButton = createXmitButton();
    layout()->addWidget(xmitButton);

    QObject::connect(toolButton, SIGNAL(clicked()), this, SLOT(showMenu()));
    QObject::connect(groupNameField, SIGNAL(editingFinished()), this, SIGNAL(
                         nameChanged()));
    QObject::connect(xmitButton, SIGNAL(toggled(bool)), this, SLOT(toggleTransmitingStatus(bool)));

    groupNameField->setAlignment(Qt::AlignHCenter);

    translateUi();
}

void LocalTrackGroupView::useSmallSpacingInLayouts(bool useSmallSpacing)
{

    QList<LocalTrackView*> tracks = getTracks<LocalTrackView*>();
    foreach (LocalTrackView *trackView, tracks) {
        trackView->useSmallSpacingInLayouts(useSmallSpacing);
    }
}

void LocalTrackGroupView::refreshStyleSheet()
{
    TrackGroupView::refreshStyleSheet();

    style()->unpolish(groupNameField);
    style()->polish(groupNameField);
}

void LocalTrackGroupView::translateUi()
{
    updateXmitButtonText();

    xmitButton->setToolTip(tr("Enable/disable your audio transmission for others"));
    xmitButton->setAccessibleDescription(toolButton->toolTip());

    toolButton->setToolTip(tr("Add or remove channels..."));
    toolButton->setAccessibleDescription(toolButton->toolTip());

    groupNameField->setPlaceholderText(tr("channel name"));
}

void LocalTrackGroupView::updateXmitButtonText()
{
    if (peakMeterOnly) {
        xmitButton->setText(tr("X"));
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
    if (!preparingToTransmit) {// users can't change xmit when is preparing
        setUnlightStatus(!checked);
        mainFrame->setTransmitingStatus(getChannelIndex(), checked);
    }
}

QPushButton *LocalTrackGroupView::createXmitButton()
{
    QPushButton *toolButton = new QPushButton();
    toolButton->setObjectName(QStringLiteral("xmitButton"));
    toolButton->setCheckable(true);
    toolButton->setChecked(true);
    return toolButton;
}

QPushButton *LocalTrackGroupView::createToolButton()
{
    QPushButton *toolButton = new QPushButton();
    toolButton->setObjectName(QStringLiteral("toolButton"));
    toolButton->setText(QStringLiteral(""));

    return toolButton;
}

void LocalTrackGroupView::closePluginsWindows()
{
    QList<LocalTrackView *> trackViews = getTracks<LocalTrackView *>();
    foreach (LocalTrackView *trackView, trackViews)
        trackView->closeAllPlugins();
}

void LocalTrackGroupView::addChannel()
{
    mainFrame->addChannelsGroup("");
}

void LocalTrackGroupView::resetTracks()
{
    foreach (LocalTrackView *track, getTracks<LocalTrackView *>())
        track->reset();
}

QMenu *LocalTrackGroupView::createPresetsSubMenu()
{
    // LOAD - using a submenu to list stored presets
    QMenu *loadMenu = new QMenu(tr("Load preset"));
    loadMenu->setIcon(QIcon(":/images/preset-load.png"));

    // adding a menu action for each stored preset
    QStringList presetsNames = Configurator::getInstance()->getPresetFilesNames(false);
    foreach (const QString &name, presetsNames) {
        QAction *presetAction = loadMenu->addAction(QString(name).replace(".json", "")); //strip the file extension from preset name
        presetAction->setData(name);// putting the preset name in the Action instance we can get this preset name inside slot 'loadPreset'
    }
    QObject::connect(loadMenu, SIGNAL(triggered(QAction *)), this, SLOT(loadPreset(QAction *)));
    loadMenu->setEnabled(!presetsNames.isEmpty());

    return loadMenu;
}

void LocalTrackGroupView::createPresetsActions(QMenu &menu)
{
    menu.addMenu(createPresetsSubMenu());

    // save preset
    QAction *addPresetActionSave = menu.addAction(QIcon(":/images/preset-save.png"), tr("Save preset"));
    QObject::connect(addPresetActionSave, SIGNAL(triggered(bool)), this, SLOT(savePreset()));

    // RESET - in case of panic
    QAction *reset = menu.addAction(QIcon(":/images/gear.png"), tr("Reset Track Controls"));
    QObject::connect(reset, SIGNAL(triggered()), this, SLOT(resetLocalTracks()));
}

void LocalTrackGroupView::createChannelsActions(QMenu &menu)
{
    QAction *addChannelAction = menu.addAction(QIcon(":/images/more.png"), tr("Add channel"));
    QObject::connect(addChannelAction, SIGNAL(triggered()), this, SLOT(addChannel()));
    addChannelAction->setEnabled(mainFrame->getChannelGroupsCount() < 2);// at this moment users can't create more channels
    if (mainFrame->getChannelGroupsCount() > 1) {
        // menu.addSeparator();
        for (int i = 2; i <= mainFrame->getChannelGroupsCount(); ++i) {
            QString channelName = mainFrame->getChannelGroupName(i-1);
            QIcon icon(":/images/less.png");
            QString text = tr("Remove channel \"%1\"").arg(channelName);
            QAction *action = menu.addAction(icon, text);
            action->setData(i-1);  // use channel index as action data
            QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(removeChannel()));
            QObject::connect(action, SIGNAL(hovered()), this, SLOT(highlightHoveredChannel()));
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
    populateMenu(menu);// populateMenu is overrided in Standalone to add subchannel actions
    menu.move(mapToGlobal(toolButton->pos() + QPoint(toolButton->width(), 0)));
    menu.exec();
}

void LocalTrackGroupView::addSubChannel()
{
    if (!trackViews.isEmpty()) {
        addTrackView(getChannelIndex());
        useSmallSpacingInLayouts(isUsingSmallSpacingInLayouts());
    }
}

bool LocalTrackGroupView::isUsingSmallSpacingInLayouts() const
{
    QList<LocalTrackView*> tracks = getTracks<LocalTrackView*>();
    if (tracks.isEmpty())
        return false;

    return tracks.first()->isUsingSmallSpacingInLayouts();
}

// +++++++++++++++++++++++++++++++++++++++++++

LocalTrackView *LocalTrackGroupView::addTrackView(long trackID)
{
    if (trackViews.size() >= MAX_SUB_CHANNELS)
        return nullptr;

    BaseTrackView *newTrack = TrackGroupView::addTrackView(trackID);

    emit trackAdded();

    return dynamic_cast<LocalTrackView *>(newTrack);
}

LocalTrackView *LocalTrackGroupView::createTrackView(long trackID)
{
    return new LocalTrackView(mainFrame->getMainController(), trackID);
}

// +++++++++++++++++++++++++++++++++++++++++++
void LocalTrackGroupView::setToWide()
{
    if (trackViews.count() <= 1) {// don't allow 2 wide subchannels
        foreach (BaseTrackView *trackView, this->trackViews)
            trackView->setToWide();
    }
}

void LocalTrackGroupView::setToNarrow()
{
    foreach (BaseTrackView *trackView, this->trackViews)
        trackView->setToNarrow();
}

void LocalTrackGroupView::highlightHoveredChannel()
{
    int channelGroupIndex = 1;// just the second channel can be highlighted at moment
    if (channelGroupIndex >= 0 && channelGroupIndex < mainFrame->getChannelGroupsCount())
        mainFrame->highlightChannelGroup(channelGroupIndex);
}

void LocalTrackGroupView::highlightHoveredSubchannel()
{
    int subChannelIndex = 1;// just the second subchannel can be highlighted at moment
    if (subChannelIndex >= 0 && subChannelIndex < trackViews.size())
        Highligther::getInstance()->highlight(trackViews.at(subChannelIndex));
}

void LocalTrackGroupView::removeSubchannel()
{
    if (trackViews.size() > 1) {// can't remove the default/first subchannel
        removeTrackView(1);// always remove the second channel
        emit trackRemoved();
    }
}

void LocalTrackGroupView::detachMainControllerInSubchannels()
{
    foreach (LocalTrackView *view, getTracks<LocalTrackView *>())
        view->detachMainController();
}

void LocalTrackGroupView::removeChannel()
{
    mainFrame->removeChannelsGroup(mainFrame->getChannelGroupsCount()-1);
}

// PRESETS
void LocalTrackGroupView::loadPreset(QAction *action)
{
    Controller::MainController *mainController = mainFrame->getMainController();
    QString presetFileName = action->data().toString();
    Persistence::Preset preset = mainController->loadPreset(presetFileName);
    if (preset.isValid()) {
        mainFrame->loadPreset(preset);

        // send the new channels to other musicians
        mainController->sendNewChannelsNames(mainFrame->getChannelsNames());
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
        mainFrame->getMainController()->savePreset(mainFrame->getInputsSettings(), text);
    }
}

void LocalTrackGroupView::resetLocalTracks()
{
    mainFrame->resetLocalChannels();//reset all local channels and subchannels
}

void LocalTrackGroupView::deletePreset()
{
    QStringList items = mainFrame->getMainController()->getPresetList();
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Remove preset"),
                                         tr("Preset:"), items, 0, false, &ok);

    // delete the file

    mainFrame->getMainController()->deletePreset(item);
}

// +++++++++++++++++++++++++++++
void LocalTrackGroupView::setPeakMeterMode(bool peakMeterOnly)
{
    if (this->peakMeterOnly != peakMeterOnly) {
        this->peakMeterOnly = peakMeterOnly;
        topPanel->setVisible(!this->peakMeterOnly);
        foreach (LocalTrackView *view, getTracks<LocalTrackView *>()) {
            view->setPeakMetersOnlyMode(peakMeterOnly, mainFrame->isRunningInMiniMode());
        }

        updateXmitButtonText();
        updateGeometry();
        adjustSize();
    }
}

void LocalTrackGroupView::togglePeakMeterOnlyMode()
{
    setPeakMeterMode(!this->peakMeterOnly);
}
