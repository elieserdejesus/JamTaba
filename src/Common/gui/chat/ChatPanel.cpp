#include "ChatPanel.h"
#include "ui_ChatPanel.h"
#include "ChatMessagePanel.h"
#include "EmojiWidget.h"
#include "EmojiManager.h"
#include "gui/TextEditorModifier.h"
#include "gui/UsersColorsPool.h"
#include "ninjam/client/User.h"

#include <QWidget>
#include <QScrollBar>
#include <QDebug>
#include <QKeyEvent>
#include <QWidget>
#include <QGridLayout>
#include <QMenu>
#include "gui/IconFactory.h"
#include "loginserver/LoginService.h"

const qint8 ChatPanel::MAX_FONT_OFFSET = 3;
const qint8 ChatPanel::MIN_FONT_OFFSET = -2;

const QColor ChatPanel::BOT_COLOR(255, 255, 255, 30);

qint8 ChatPanel::fontSizeOffset = 0;

QList<ChatPanel *> ChatPanel::instances;

ChatPanel::ChatPanel(const QStringList &botNames, UsersColorsPool *colorsPool,
                        TextEditorModifier *chatInputModifier, EmojiManager *emojiManager) :
    QWidget(nullptr),
    ui(new Ui::ChatPanel),
    emojiManager(emojiManager),
    botNames(botNames),
    autoTranslating(false),
    colorsPool(colorsPool),
    unreadedMessages(0),
    on(false)
{
    ui->setupUi(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(ui->scrollContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    ui->scrollContent->setLayout(contentLayout);

    ui->topicLabel->setVisible(false);

    // disable blue border when QLineEdit has focus in mac
    ui->chatText->setAttribute(Qt::WA_MacShowFocusRect, 0);

    previousVerticalScrollBarMaxValue = ui->chatScroll->verticalScrollBar()->value();

    emojiWidget = new EmojiWidget(emojiManager, this);
    emojiWidget->setVisible(false);
    qobject_cast<QVBoxLayout *>(layout())->insertWidget(layout()->count()-2, emojiWidget);
    emojiWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);

    auto emojiIcon = IconFactory::createChatEmojiIcon(Qt::black, on);
    emojiAction = ui->chatText->addAction(emojiIcon, QLineEdit::LeadingPosition);

    if (chatInputModifier) {
        bool finishEditorPressingReturnKey = false;
        chatInputModifier->modify(ui->chatText, finishEditorPressingReturnKey);
    }

    ui->scrollContent->installEventFilter(this);

    setupSignals();

    instances.append(this);

    auto root = new QTreeWidgetItem(ui->treeWidget, QStringList());
    root->setFirstColumnSpanned(true); // the root col span
    ui->treeWidget->addTopLevelItem(root);

    connect(ui->treeWidget, &QTreeWidget::collapsed, [=](){
        ui->treeWidget->setMaximumHeight(20);
    });

    connect(ui->treeWidget, &QTreeWidget::expanded, [=](){
        auto root = ui->treeWidget->topLevelItem(0);
        Q_ASSERT(root);
        if (root->childCount() > 0)
            ui->treeWidget->setMaximumHeight(150);
    });

    connect(ui->treeWidget, &QTreeWidget::clicked, [=](QModelIndex index){
        auto root = ui->treeWidget->topLevelItem(0);
        Q_ASSERT(root);
        if (root->isExpanded())
            ui->treeWidget->collapse(index);
        else
            ui->treeWidget->expand(index);
    });

    ui->treeWidget->setMaximumHeight(20);

    ui->treeWidget->setVisible(false);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &ChatPanel::showContextMenu);

    ui->buttonOnOff->setIcon(IconFactory::createChatOnOffIcon(Qt::black));
    ui->buttonOnOff->setChecked(on);
    connect(ui->buttonOnOff, &QPushButton::toggled, this, &ChatPanel::toggleOnOff);

    updatePlaceHolderText();
    updateEmojiIcon();

    showConnectedUsersWidget(false);
}

void ChatPanel::turnOn()
{
    if (!on)
        toggleOnOff();
}

void ChatPanel::turnOff()
{
    if (on)
        toggleOnOff();
}

bool ChatPanel::isOn() const
{
    return on;
}

void ChatPanel::toggleOnOff()
{
    on = !on;

    setInputsStatus(on);

    if (on)
        emit turnedOn();
    else
        emit turnedOff();

    updatePlaceHolderText();
    updateEmojiIcon();
}

void ChatPanel::updatePlaceHolderText()
{
    if (on)
        ui->chatText->setPlaceholderText(tr("type here ..."));
    else
        ui->chatText->setPlaceholderText(tr("chat is off"));
}

void ChatPanel::updateEmojiIcon()
{
    emojiAction->setIcon(IconFactory::createChatEmojiIcon(tintColor, on));
}

void ChatPanel::hideOnOffButton()
{
    ui->buttonOnOff->setVisible(false);
}

void ChatPanel::showContextMenu(const QPoint &pos)
{
    auto item = ui->treeWidget->itemAt(pos);
    auto root = ui->treeWidget->topLevelItem(0);
    if (!item || item == root)
        return;

    auto userName = item->text(0);

    auto userIp = item->data(0, Qt::UserRole + 1).toString();

    auto userFullName = QString("%1@%2").arg(userName).arg(userIp);

    QMenu menu;

    emit connectedUserContextMenuActivated(menu, userFullName);

    if (!menu.isEmpty()) {
        auto menuPos = ui->treeWidget->viewport()->mapToGlobal(pos);
        menu.exec(menuPos);
    }
}

void ChatPanel::showConnectedUsersWidget(bool show)
{
    ui->verticalSpacerBottom->changeSize(0, show ? 12 : 0);
    ui->treeWidget->setVisible(show);
}

void ChatPanel::updateUsersLocation(const QString &ip, const login::Location &location)
{
    auto root = ui->treeWidget->topLevelItem(0);
    for (int i = 0; i < root->childCount(); ++i) {
        auto item = root->child(i);
        auto itemIP = item->data(0, Qt::UserRole + 1).toString();
        if (ninjam::client::maskIP(itemIP) == ninjam::client::maskIP(ip)) {
            setItemCountryDetails(item, location);
        }
    }
}

void ChatPanel::setItemCountryDetails(QTreeWidgetItem *item, const login::Location &location)
{
    auto icon = QIcon(QString(":/flags/flags/%1.png").arg(location.countryCode.toLower()));

    auto countryCollumn = 1;

    item->setIcon(countryCollumn, icon);
    item->setText(countryCollumn, location.countryName);
}

void ChatPanel::setConnectedUserBlockedStatus(const QString &userFullName, bool blocked)
{
    auto root = ui->treeWidget->topLevelItem(0);
    Q_ASSERT(root);

    for (int i = 0; i < root->childCount(); ++i) {
        auto item = root->child(i);
        auto itemIp = item->data(0, Qt::UserRole + 1).toString();
        auto userIp = ninjam::client::extractUserIP(userFullName);
        auto userName = ninjam::client::extractUserName(userFullName);
        if (itemIp == userIp && userName == item->text(0)) {
            QIcon icon(blocked ? ":/images/chat_blocked.png" : QString());
            item->setIcon(0, icon);
            return;
        }
    }
}

void ChatPanel::setConnectedUsers(const QStringList &usersNames)
{
    updateConnectedUsersTitleString(usersNames.count());

    auto root = ui->treeWidget->topLevelItem(0);
    Q_ASSERT(root);

    while (root->childCount() > 0) {
        delete root->takeChild(0);
    }

    for (const auto &userFullName : usersNames) {
        auto name = ninjam::client::extractUserName(userFullName);
        auto ip = ninjam::client::extractUserIP(userFullName);
        auto item = new QTreeWidgetItem(root, QStringList(name));
        item->setData(0, Qt::UserRole + 1, ip);
        setItemCountryDetails(item, login::Location()); // unknown location
        root->addChild(item);
    }

    //ui->treeWidget->setVisible(usersNames.size() > 1);
    ui->treeWidget->setVisible(true);
    root->setExpanded(usersNames.size() > 1); // issue #1107
}

QList<QString> ChatPanel::getConnectedUsers() const
{
    QList<QString> users;
    auto root = ui->treeWidget->topLevelItem(0);
    for (int i = 0; i < root->childCount(); ++i) {
        auto item = root->child(i);
        auto ip = item->data(0, Qt::UserRole + 1).toString();
        auto name = item->text(0);
        users.append(QString("%1@%2").arg(name).arg(ip));
    }
    return users;
}

void ChatPanel::updateConnectedUsersTitleString(quint32 connectedUsers)
{
    auto root = ui->treeWidget->topLevelItem(0);
    Q_ASSERT(root);

    root->setText(0, tr("Connected Users (%1)").arg(connectedUsers));
}

void ChatPanel::setupSignals()
{
    connect(ui->chatText, &QLineEdit::returnPressed, this, &ChatPanel::sendNewMessage);

    connect(ui->chatText, &QLineEdit::returnPressed, [=]() {
        // auto scroll when user is typing new messages
        int scrollValue = ui->chatScroll->verticalScrollBar()->value();
        int scrollMaximum = ui->chatScroll->verticalScrollBar()->maximum();
        if (scrollValue < scrollMaximum) { // need auto scroll?
            ui->chatScroll->verticalScrollBar()->setValue(scrollMaximum);
        }
    });

    // this event is used to auto scroll down when new messages are added
    connect(ui->chatScroll->verticalScrollBar(), &QScrollBar::rangeChanged, this, &ChatPanel::autoScroll);

    connect(ui->buttonClear, &QPushButton::clicked, this, &ChatPanel::clearMessages);

    connect(ui->buttonAutoTranslate, &QPushButton::clicked, this, &ChatPanel::toggleAutoTranslate);

    connect(emojiAction, &QAction::triggered, [=]() {
        emojiWidget->setVisible(!emojiWidget->isVisible());
    });

    connect(emojiWidget, &EmojiWidget::emojiSelected, [=](const QString &emojiCode) {
        QString newText = ui->chatText->text();
        newText.insert(ui->chatText->cursorPosition(), EmojiManager::emojiCodeToUtf8(emojiCode));
        ui->chatText->setText(newText);
        ui->chatText->setFocus();
    });

    connect(ui->toolButtonPlus, &QToolButton::clicked, this, &ChatPanel::increaseFontSize);
    connect(ui->toolButtonMinus, &QToolButton::clicked, this, &ChatPanel::decreaseFontSize);
}

void ChatPanel::setFontSizeOffset(qint8 sizeOffset)
{
    ChatPanel::fontSizeOffset = sizeOffset;

    for (auto chatPanel : ChatPanel::instances)
        chatPanel->setMessagesFontSizeOffset(fontSizeOffset);
}

void ChatPanel::setMessagesFontSizeOffset(qint8 offset)
{
    auto messages = ui->scrollContent->findChildren<ChatMessagePanel *>();
    for (auto msg : messages)
        msg->setFontSizeOffset(offset);
}

void ChatPanel::increaseFontSize()
{
    if (ChatPanel::fontSizeOffset < ChatPanel::MAX_FONT_OFFSET) {
        ChatPanel::setFontSizeOffset(ChatPanel::fontSizeOffset + 1);

        emit fontSizeOffsetEdited(ChatPanel::fontSizeOffset);
    }
}

void ChatPanel::decreaseFontSize()
{
    if (ChatPanel::fontSizeOffset > ChatPanel::MIN_FONT_OFFSET) {
        ChatPanel::setFontSizeOffset(ChatPanel::fontSizeOffset - 1);

        emit fontSizeOffsetEdited(ChatPanel::fontSizeOffset);
    }
}

bool ChatPanel::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->scrollContent && e->type() == QEvent::Resize) {
        ui->scrollContent->setMaximumWidth(ui->chatScroll->viewport()->width());
    }

    return QWidget::eventFilter(o, e);
}

void ChatPanel::setTintColor(const QColor &color)
{
    emojiAction->setIcon(IconFactory::createChatEmojiIcon(color, on));

    ui->toolButtonPlus->setIcon(IconFactory::createFontSizeIncreaseIcon(color));
    ui->toolButtonMinus->setIcon(IconFactory::createFontSizeDecreaseIcon(color));
    ui->labelFontSize->setPixmap(IconFactory::createFontSizePixmap(color));

    ui->buttonOnOff->setIcon(IconFactory::createChatOnOffIcon(color));

    tintColor = color;
}

bool ChatPanel::inputsAreEnabled() const
{
    return ui->chatText->isEnabled();
}

void ChatPanel::setInputsStatus(bool enabled)
{
    ui->chatText->setEnabled(enabled);
    ui->buttonAutoTranslate->setEnabled(enabled);
    ui->buttonClear->setEnabled(enabled);
}

void ChatPanel::setTopicMessage(const QString &topic)
{
    ui->topicLabel->setText(topic);

    ui->topicLabel->setVisible(!topic.isEmpty());
}

void ChatPanel::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updatePlaceHolderText();

        auto root = ui->treeWidget->topLevelItem(0);
        if (root)
            updateConnectedUsersTitleString(root->childCount());
    }

    QWidget::changeEvent(e);
}

void ChatPanel::showEvent(QShowEvent *ev)
{
    Q_UNUSED(ev)

    setUnreadedMessages(0);
}

void ChatPanel::createServerInviteButton(const QString &serverIP, quint16 serverPort)
{
    auto inviteButton = new ServerInviteButton(serverIP, serverPort, this);

    inviteButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    ui->scrollContent->layout()->addWidget(inviteButton);
    ui->scrollContent->layout()->setAlignment(inviteButton, Qt::AlignRight);
    connect(inviteButton, &QPushButton::clicked, [=](){
        emit userAcceptingServerInvite(serverIP, serverPort);
    });
}

void ChatPanel::createVoteButton(const QString &voteType, quint32 value, quint32 expireTime)
{
    QPushButton *voteButton = new NinjamVoteButton(voteType, value, expireTime);
    voteButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    ui->scrollContent->layout()->addWidget(voteButton);
    ui->scrollContent->layout()->setAlignment(voteButton, Qt::AlignRight);
    connect(voteButton, &QPushButton::clicked, this, &ChatPanel::confirmVote);
}

void ChatPanel::addBpiVoteConfirmationMessage(quint32 newBpiValue, quint32 expireTime)
{
    createVoteButton("BPI", newBpiValue, expireTime);
}

void ChatPanel::addBpmVoteConfirmationMessage(quint32 newBpmValue, quint32 expireTime)
{
    createVoteButton("BPM", newBpmValue, expireTime);
}

void ChatPanel::confirmVote()
{
    NinjamVoteButton *voteButton = static_cast<NinjamVoteButton *>(QObject::sender());
    if (voteButton->isBpiVote())
        emit userConfirmingVoteToBpiChange(voteButton->getVoteValue());
    else if (voteButton->isBpmVote())
        emit userConfirmingVoteToBpmChange(voteButton->getVoteValue());

    voteButton->parentWidget()->layout()->removeWidget(voteButton);
    voteButton->deleteLater();
}

// ++++++++++++++++++++++++++++++++++



void ChatPanel::addChordProgressionConfirmationMessage(const ChordProgression &progression)
{
    QString buttonText = tr("Use/load the chords above");
    QPushButton *chordProgressionButton = new ChordProgressionConfirmationButton(buttonText,
                                                                                 progression);
    chordProgressionButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    ui->scrollContent->layout()->addWidget(chordProgressionButton);
    ui->scrollContent->layout()->setAlignment(chordProgressionButton, Qt::AlignRight);
    connect(chordProgressionButton, &QPushButton::clicked, this, &ChatPanel::confirmChordProgression);
}

// +++++++++++++++++++++++++++++++
void ChatPanel::confirmChordProgression()
{
    ChordProgressionConfirmationButton *chordProgressionButton
        = qobject_cast<ChordProgressionConfirmationButton *>(QObject::sender());

    emit userConfirmingChordProgression(chordProgressionButton->getChordProgression());

    chordProgressionButton->parentWidget()->layout()->removeWidget(chordProgressionButton);
    chordProgressionButton->deleteLater();
}

// +++++++++++++++
void ChatPanel::autoScroll(int min, int max)
{
    Q_UNUSED(min)


    // used to auto scroll down to keep the last added message visible

    int currentValue = ui->chatScroll->verticalScrollBar()->value();
    if (currentValue >= previousVerticalScrollBarMaxValue) { // avoid auto scroll if the vertical scroll bar is not in max value position (use is scrolling up)
        ui->chatScroll->verticalScrollBar()->setValue(max + 10);
    }

    previousVerticalScrollBarMaxValue = max;
}

void ChatPanel::sendNewMessage()
{
    QString messageText = ui->chatText->text();
    if (!messageText.isEmpty()) {
        emit userSendingNewMessage(messageText);
        ui->chatText->clear();
    }
}

void ChatPanel::updateMessagesGeometry()
{
    QList<ChatMessagePanel *> messages = ui->scrollContent->findChildren<ChatMessagePanel *>();
    foreach (ChatMessagePanel *msg, messages) {
        msg->setMaximumWidth(ui->chatScroll->viewport()->width() - 20);
        msg->updateGeometry();
    }
}

void ChatPanel::showTranslationProgressFeedback()
{
    if(!autoTranslating){
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();
    }
}

void ChatPanel::hideTranslationProgressFeedback()
{
    if(!autoTranslating)
        QApplication::restoreOverrideCursor();
}

void ChatPanel::addLastChordsMessage(const QString &userName, const QString &message, QColor textColor, QColor backgroundColor)
{
    ChatMessagePanel *msgPanel = new ChatMessagePanel(ui->scrollContent, userName, message,
                                                      backgroundColor, textColor, false, false);

    addMessagePanelInLayout(msgPanel, Qt::AlignLeft);

}

void ChatPanel::addMessage(const QString &localUserName, const QString &msgAuthorFullName, const QString &msgText, bool showTranslationButton, bool showBlockButton)
{

    QString fullName = !msgAuthorFullName.isEmpty() ? msgAuthorFullName : "JamTaba";

    QColor backgroundColor = getUserColor(fullName);

    bool isBot = backgroundColor == BOT_COLOR;
    bool isLocalUser = ninjam::client::extractUserName(msgAuthorFullName) == localUserName;

    QColor textColor = Qt::black;
    QColor userNameBackgroundColor = backgroundColor;

    ChatMessagePanel *msgPanel = new ChatMessagePanel(this, fullName, msgText,
                                                      userNameBackgroundColor, textColor, showTranslationButton, showBlockButton, emojiManager);

    connect(msgPanel, &ChatMessagePanel::startingTranslation, this, &ChatPanel::showTranslationProgressFeedback);
    connect(msgPanel, &ChatMessagePanel::translationFinished, this, &ChatPanel::hideTranslationProgressFeedback);

    connect(msgPanel, &ChatMessagePanel::blockingUser, this, &ChatPanel::userBlockingChatMessagesFrom);

    msgPanel->setPrefferedTranslationLanguage(autoTranslationLanguage);
    msgPanel->setShowArrow(!isBot);
    if (!isBot && isLocalUser) // local user messages are showed in right side
        msgPanel->setArrowSide(ChatMessagePanel::RightSide);

    Qt::Alignment alignment = Qt::AlignLeft;
    if (isLocalUser)
        alignment = Qt::AlignRight;

    addMessagePanelInLayout(msgPanel, alignment);

    msgPanel->setFontSizeOffset(ChatPanel::fontSizeOffset);

    bool canAutoTranslate = autoTranslating && !isLocalUser; // local user messages are not auto translated
    if (canAutoTranslate)
        msgPanel->translate(); // request the auto translation

    if (!isVisible()) {
        setUnreadedMessages(unreadedMessages + 1);
    }
}

void ChatPanel::setUnreadedMessages(uint unreaded)
{
    if (unreaded != unreadedMessages) {
        unreadedMessages = unreaded;

        emit unreadedMessagesChanged(unreadedMessages);
    }
}

void ChatPanel::addMessagePanelInLayout(ChatMessagePanel *msgPanel, Qt::Alignment alignment)
{
    ui->scrollContent->layout()->addWidget(msgPanel);
    if (ui->scrollContent->layout()->count() > MAX_MESSAGES) {
        // remove the first widget
        QList<ChatMessagePanel *> panels = ui->scrollContent->findChildren<ChatMessagePanel *>();
        ui->scrollContent->layout()->removeWidget(panels.first());
        delete panels.first();
    }
    ui->scrollContent->layout()->setAlignment(Qt::AlignTop);
    ui->scrollContent->layout()->setAlignment(msgPanel, Qt::AlignTop | alignment);
}

// +++++++++++++++++++++++++++++++++++=
QColor ChatPanel::getUserColor(const QString &userName)
{
    if (botNames.contains(userName, Qt::CaseInsensitive) || userName.isEmpty()
        || userName.toLower() == "jamtaba")
        return BOT_COLOR;

    Q_ASSERT(colorsPool);
    return  colorsPool->get(userName);
}

ChatPanel::~ChatPanel()
{
    instances.removeOne(this);

    delete ui;
}

void ChatPanel::removeMessagesFrom(const QString &userFullName)
{
    // remove message panels from user 'userName'
    auto panels = ui->scrollContent->findChildren<ChatMessagePanel *>();
    for (auto msgPanel : panels) {
        if (msgPanel->getUserFullName() == userFullName) {
            ui->scrollContent->layout()->removeWidget(msgPanel);
            msgPanel->deleteLater();
        }
    }
}

void ChatPanel::clearMessages()
{
    // remove message panels
    QList<ChatMessagePanel *> panels = ui->scrollContent->findChildren<ChatMessagePanel *>();
    foreach (ChatMessagePanel *msgPanel, panels) {
        ui->scrollContent->layout()->removeWidget(msgPanel);
        msgPanel->deleteLater();
    }

    // remove Vote and 'load chords' buttons
    QList<QPushButton *> buttons = ui->scrollContent->findChildren<QPushButton *>();
    foreach (QPushButton *button, buttons) {
        ui->scrollContent->layout()->removeWidget(button);
        button->deleteLater();
    }
}

void ChatPanel::setPreferredTranslationLanguage(const QString &targetLanguage)
{
    QString languageCode = targetLanguage.toLower();
    if (languageCode.size() > 2) {
        languageCode = targetLanguage.left(2); //using just the 2 first letters in lower case
    }
    if (languageCode != autoTranslationLanguage) {
        autoTranslationLanguage = languageCode;
        QList<ChatMessagePanel *> panels = ui->scrollContent->findChildren<ChatMessagePanel *>();
        foreach (ChatMessagePanel *msgPanel, panels) {
            msgPanel->setPrefferedTranslationLanguage(languageCode);
        }
    }
}

void ChatPanel::toggleAutoTranslate()
{
    this->autoTranslating = ui->buttonAutoTranslate->isChecked();
}
