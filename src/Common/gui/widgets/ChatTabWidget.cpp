#include "ChatTabWidget.h"

#include <QBoxLayout>
#include <QStyle>
#include <QRegularExpression>

#include "ChatPanel.h"
#include "UsersColorsPool.h"
#include "MainController.h"

ChatTabWidget::ChatTabWidget(QWidget *parent, controller::MainController *mainController, UsersColorsPool *colorsPool) :
    QFrame(parent),
    tabBar(new QTabBar(this)),
    stackWidget(new QStackedWidget(this)),
    botNames(mainController->getBotNames()),
    colorsPool(colorsPool),
    mainChat(nullptr),
    mainController(mainController)
{

    QBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    tabBar->setShape(QTabBar::RoundedNorth);
    tabBar->setDrawBase(false); // not drawing the QTabBar base line
    tabBar->setExpanding(false);
    tabBar->setTabsClosable(true);

    layout->addWidget(tabBar);
    layout->addWidget(stackWidget);

    layout->setAlignment(tabBar, Qt::AlignTop | Qt::AlignLeft);

    connect(tabBar, &QTabBar::currentChanged, this, [=](int index) {

        if (!stackWidget->isVisible())
            collapse(false);

        stackWidget->setCurrentIndex(index);

    });

    connect(tabBar, &QTabBar::tabCloseRequested, this, &ChatTabWidget::closeChatTab);

    setObjectName(QStringLiteral("chatTabWidget"));

    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

    stackWidget->setMinimumWidth(230); // TODO Refactoring: remove these 'Magic Numbers'
}

void ChatTabWidget::clear()
{
    while (tabBar->count() > 0) // remove all tabs
        tabBar->removeTab(0);

    while (stackWidget->count() > 0){
        auto widget = stackWidget->currentWidget();
        stackWidget->removeWidget(widget);
        widget->deleteLater();
    }

    mainChat = nullptr;
}

ChatPanel *ChatTabWidget::createPublicChat(const QString &jamTabaChatBotName, TextEditorModifier *textEditorModifier)
{
    if (mainChat)
        return mainChat;

    //add main chat
    tabBar->addTab(tr("Chat")); // main tab

    auto botNames = mainController->getBotNames();
    auto emojiManager = mainController->getEmojiManager();
    mainChat = new ChatPanel(jamTabaChatBotName, botNames, colorsPool, textEditorModifier, emojiManager);
    stackWidget->addWidget(mainChat);

    connect(mainChat, &ChatPanel::unreadedMessagesChanged, this, [=](uint unreaded) {

        updatePublicChatTabTitle(unreaded);
    });

    removeTabCloseButton(0); // the main chat is not closable

    updatePublicChatTabTitle(); // set and translate the chat tab title

    mainChat->setPreferredTranslationLanguage(mainController->getTranslationLanguage());

    return mainChat;

}

void ChatTabWidget::updatePublicChatTabTitle(uint unreadedMessages)
{
    int chatTabIndex = 0; // assuming main chat is always the first tab
    QString text = tr("Chat");
    if (unreadedMessages > 0)
        text = QString("(%1) %2").arg(unreadedMessages).arg(text);

    tabBar->setTabText(chatTabIndex, text);
}

void ChatTabWidget::closeChatTab(int index)
{
    if (index == 0)  // the first chat (main chat) is not closeable
        return;

    tabBar->removeTab(index);

    if (index < stackWidget->count()) {
        auto widget = stackWidget->widget(index);
        stackWidget->removeWidget(widget);
        widget->deleteLater();
    }

}

void ChatTabWidget::setPreferredTranslationLanguage(const QString locale)
{
    for (int i = 0; i < stackWidget->count(); ++i) {
        static_cast<ChatPanel *>(stackWidget->widget(i))->setPreferredTranslationLanguage(locale);
    }
}

bool ChatTabWidget::isCollapsed() const
{
    return !stackWidget->isVisible();
}


void ChatTabWidget::toggleCollapse()
{
    collapse(!isCollapsed());
}

void ChatTabWidget::collapse(bool collapse)
{

    stackWidget->setVisible(!collapse);

    tabBar->setShape(collapse ? QTabBar::RoundedEast : QTabBar::RoundedNorth);

    setSizePolicy(QSizePolicy(collapse ? QSizePolicy::Maximum : QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));

    emit collapsedChanged(collapse);

    setProperty("collapsed", collapse);

    style()->unpolish(this);
    style()->polish(this);
    style()->unpolish(tabBar);
    style()->polish(tabBar);
    style()->unpolish(stackWidget);
    style()->polish(stackWidget);

    updateGeometry();
}

ChatPanel *ChatTabWidget::createPrivateChat(const QString &remoteUserName, const QString &userIP, TextEditorModifier *textModifider, bool focusNewChat)
{
    QString userFullName = remoteUserName + "@" + userIP;

    auto chatPanel = getPrivateChat(userFullName);

    if (!chatPanel) {     // create new chat
        auto botNames = mainController->getBotNames();
        chatPanel = new ChatPanel(userFullName, botNames, colorsPool, textModifider, emojiManager);
        int tabIndex = tabBar->addTab(remoteUserName);
        stackWidget->addWidget(chatPanel);

        connect(chatPanel, &ChatPanel::unreadedMessagesChanged, this, [=](uint unreaded) {
            updatePrivateChatTabTitle(tabIndex, unreaded);
        });

        if (focusNewChat)
            tabBar->setCurrentIndex(tabIndex);
    }

    return chatPanel;
}

void ChatTabWidget::updatePrivateChatTabTitle(int chatIndex, uint unreadedMessages)
{
    Q_ASSERT(chatIndex > 0); // index ZERO is the public chat

    auto chatPanel = static_cast<ChatPanel *>(stackWidget->widget(chatIndex));
    if (!chatPanel)
        return;

    QString text = chatPanel->getUserFullName().replace(QRegularExpression("@.+"), "");
    if (unreadedMessages > 0)
        text = QString("(%1) %2").arg(unreadedMessages).arg(text);

    tabBar->setTabText(chatIndex, text);
}


bool ChatTabWidget::contains(const QString &userFullName) const
{
    for (auto chat : getChats())
        if (chat->getUserFullName() == userFullName)
            return true;

    return false;
}

ChatPanel *ChatTabWidget::getPrivateChat(const QString &userFullName)
{
    for (auto chat : getChats())
        if (chat->getUserFullName() == userFullName)
            return chat;

    return nullptr;
}


QList<ChatPanel *>ChatTabWidget::getChats() const
{
    QList<ChatPanel *> chats;
    for (int i = 0; i < stackWidget->count(); ++i) {
        chats << static_cast<ChatPanel *>(stackWidget->widget(i));
    }

    return chats;
}

ChatPanel *ChatTabWidget::getFocusedChatPanel() const
{
    return static_cast<ChatPanel *>(stackWidget->currentWidget());
}

void ChatTabWidget::removeTabCloseButton(int buttonIndex)
{
    if (!tabBar)
        return;

    auto tabButton = tabBar->tabButton(buttonIndex, QTabBar::RightSide);

    if (!tabButton) // try get the tabBar in left side (MAC OSX)
        tabButton = tabBar->tabButton(buttonIndex, QTabBar::LeftSide);

    if (tabButton) {
        tabButton->resize(0, 0);
        tabButton->hide();
    }
}
