#include "ChatPanel.h"
#include "ui_ChatPanel.h"
#include "ChatMessagePanel.h"
#include "EmojiWidget.h"

#include <QWidget>
#include <QScrollBar>
#include <QDebug>
#include <QKeyEvent>
#include <QWidget>
#include <QGridLayout>
#include "IconFactory.h"

#include "TextEditorModifier.h"
#include "UsersColorsPool.h"
#include "EmojiManager.h"
#include "ninjam/User.h"

const QColor ChatPanel::BOT_COLOR(255, 255, 255, 30);

ChatPanel::ChatPanel(const QStringList &botNames, UsersColorsPool *colorsPool,
                        TextEditorModifier *chatInputModifier, EmojiManager *emojiManager) :
    QWidget(nullptr),
    ui(new Ui::ChatPanel),
    emojiManager(emojiManager),
    botNames(botNames),
    autoTranslating(false),
    colorsPool(colorsPool),
    unreadedMessages(0),
    tintColor(Qt::black)
{
    ui->setupUi(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(ui->scrollContent);
    contentLayout->setContentsMargins(3, 3, 3, 3);
    ui->scrollContent->setLayout(contentLayout);

    connect(ui->chatText, &QLineEdit::returnPressed, this, &ChatPanel::sendNewMessage);

    connect(ui->chatText, &QLineEdit::returnPressed, [=](){
        //auto scroll when user is typing new messages
        int scrollValue = ui->chatScroll->verticalScrollBar()->value();
        int scrollMaximum = ui->chatScroll->verticalScrollBar()->maximum();
        if (scrollValue < scrollMaximum) { // need auto scroll?
            ui->chatScroll->verticalScrollBar()->setValue(scrollMaximum);
        }
    });

    ui->topicLabel->setVisible(false);

    // this event is used to auto scroll down when new messages are added
    connect(ui->chatScroll->verticalScrollBar(), &QScrollBar::rangeChanged, this, &ChatPanel::autoScroll);

    connect(ui->buttonClear, &QPushButton::clicked, this, &ChatPanel::clearMessages);

    connect(ui->buttonAutoTranslate, &QPushButton::clicked, this, &ChatPanel::toggleAutoTranslate);

    // disable blue border when QLineEdit has focus in mac
    ui->chatText->setAttribute(Qt::WA_MacShowFocusRect, 0);

    previousVerticalScrollBarMaxValue = ui->chatScroll->verticalScrollBar()->value();

    emojiWidget = new EmojiWidget(emojiManager, this);
    emojiWidget->setVisible(false);
    ui->gridLayout->addWidget(emojiWidget, ui->gridLayout->rowCount(), 0, 1, ui->gridLayout->columnCount());
    emojiWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);

    auto emojiIcon = IconFactory::createChatEmojiIcon(tintColor);
    emojiAction = ui->chatText->addAction(emojiIcon, QLineEdit::LeadingPosition);

    connect(emojiAction, &QAction::triggered, [=](){
        emojiWidget->setVisible(!emojiWidget->isVisible());
    });

    connect(emojiWidget, &EmojiWidget::emojiSelected, [=](const QString &emojiCode){

        QString newText = ui->chatText->text();
        newText.insert(ui->chatText->cursorPosition(), EmojiManager::emojiCodeToUtf8(emojiCode));
        ui->chatText->setText(newText);
        ui->chatText->setFocus();
    });

    if (chatInputModifier) {
        bool finishEditorPressingReturnKey = false;
        chatInputModifier->modify(ui->chatText, finishEditorPressingReturnKey);
    }

    ui->scrollContent->installEventFilter(this);
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
    emojiAction->setIcon(IconFactory::createChatEmojiIcon(color));
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
    }

    QWidget::changeEvent(e);
}

void ChatPanel::showEvent(QShowEvent *ev)
{
    Q_UNUSED(ev)

    setUnreadedMessages(0);
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

void ChatPanel::addMessage(const QString &localUserName, const QString &msgAuthor, const QString &msgText, bool showTranslationButton, bool showBlockButton)
{

    QString name = !msgAuthor.isEmpty() ? msgAuthor : "JamTaba";

    QColor backgroundColor = getUserColor(name);

    bool isBot = backgroundColor == BOT_COLOR;
    bool isLocalUser = msgAuthor == ninjam::extractUserName(localUserName);

    QColor textColor = isBot ? QColor(50, 50, 50) : QColor(0, 0, 0);
    QColor userNameBackgroundColor = backgroundColor;

    ChatMessagePanel *msgPanel = new ChatMessagePanel(ui->scrollContent, name, msgText,
                                                      userNameBackgroundColor, textColor, showTranslationButton, showBlockButton, emojiManager);

    connect(msgPanel, SIGNAL(startingTranslation()), this, SLOT(showTranslationProgressFeedback()));
    connect(msgPanel, SIGNAL(translationFinished()), this, SLOT(hideTranslationProgressFeedback()));

    connect(msgPanel, &ChatMessagePanel::blockingUser, this, &ChatPanel::userBlockingChatMessagesFrom);

    msgPanel->setPrefferedTranslationLanguage(this->autoTranslationLanguage);
    msgPanel->setShowArrow(!isBot);
    if (!isBot && isLocalUser) // local user messages are showed in right side
        msgPanel->setArrowSide(ChatMessagePanel::RightSide);

    Qt::Alignment alignment = Qt::AlignLeft;
    if (isLocalUser)
        alignment = Qt::AlignRight;

    addMessagePanelInLayout(msgPanel, alignment);

    if (autoTranslating)
        msgPanel->translate();// request the translation

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
    delete ui;
}

void ChatPanel::removeMessagesFrom(const QString &userName)
{
    // remove message panels from user 'userName'
    QList<ChatMessagePanel *> panels = ui->scrollContent->findChildren<ChatMessagePanel *>();
    foreach (ChatMessagePanel *msgPanel, panels) {
        if (msgPanel->getUserName() == userName ) {
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
