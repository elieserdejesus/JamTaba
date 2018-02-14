#include "ChatTabWidget.h"

#include "MainController.h"

#include <QBoxLayout>
#include <QStyle>
#include <QRegularExpression>

#include "gui/chat/ChatPanel.h"
#include "gui/UsersColorsPool.h"
#include "ninjam/client/User.h"

using controller::MainController;

ChatTabWidget::ChatTabWidget(QWidget *parent, MainController *mainController, UsersColorsPool *colorsPool) :
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

    connect(tabBar, &QTabBar::currentChanged, this, &ChatTabWidget::changeCurrentTab);

    connect(tabBar, &QTabBar::tabBarClicked, [=](int index){
        if (tabBar->count() < 2) { // just onde tab?
            toggleCollapse();
        }
        else {
            changeCurrentTab(index);
            if (isCollapsed()) // user clicking in a collapsed tab
                toggleCollapse();
        }
    });

    connect(tabBar, &QTabBar::tabCloseRequested, this, &ChatTabWidget::closeChatTab);

    setObjectName(QStringLiteral("chatTabWidget"));

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    stackWidget->setMinimumWidth(230); // TODO Refactoring: remove these 'Magic Numbers'
}

void ChatTabWidget::changeCurrentTab(int tabIndex)
{
    if (tabBar->currentIndex() != tabIndex) {
        tabBar->setCurrentIndex(tabIndex);
        return;
    }

    stackWidget->setCurrentIndex(tabIndex);
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

    for (auto chat : privateChats.values())
        chat->deleteLater();

    privateChats.clear();
}

ChatPanel *ChatTabWidget::createPublicChat(TextEditorModifier *textEditorModifier)
{
    if (mainChat)
        return mainChat;

    //add main chat
    tabBar->addTab(tr("Chat")); // main tab

    auto botNames = mainController->getBotNames();
    auto emojiManager = mainController->getEmojiManager();
    mainChat = new ChatPanel(botNames, colorsPool, textEditorModifier, emojiManager);
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
        auto chatPanel = static_cast<ChatPanel *>(stackWidget->widget(index));
        if (chatPanel) {
            stackWidget->removeWidget(chatPanel);
            privateChats.remove(chatPanel->getRemoteUserFullName());
            chatPanel->deleteLater();
        }
        else {
            qCritical() << "Error deleting chatPanel!";
        }
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
    QString remoteUserFullName = remoteUserName + "@" + userIP;

    auto chatPanel = getPrivateChat(remoteUserFullName);

    if (!chatPanel) {     // create new chat
        auto botNames = mainController->getBotNames();
        auto emojiManager = mainController->getEmojiManager();
        chatPanel = new ChatPanel(botNames, colorsPool, textModifider, emojiManager);
        chatPanel->setRemoteUserFullName(remoteUserFullName);
        int tabIndex = tabBar->addTab(remoteUserName);
        stackWidget->addWidget(chatPanel);

        connect(chatPanel, &ChatPanel::unreadedMessagesChanged, this, [=](uint unreaded) {
            updatePrivateChatTabTitle(tabIndex, unreaded, remoteUserName);
        });

        if (focusNewChat)
            tabBar->setCurrentIndex(tabIndex);

        privateChats.insert(remoteUserFullName, chatPanel);
    }

    return chatPanel;
}

void ChatTabWidget::updatePrivateChatTabTitle(int chatIndex, uint unreadedMessages, const QString &remoteUserName)
{
    Q_ASSERT(chatIndex > 0); // index ZERO is the public chat

    auto chatPanel = static_cast<ChatPanel *>(stackWidget->widget(chatIndex));
    if (!chatPanel)
        return;

    QString tabText = ninjam::client::extractUserName(remoteUserName);
    if (unreadedMessages > 0)
        tabText = QString("(%1) %2").arg(unreadedMessages).arg(tabText);

    tabBar->setTabText(chatIndex, tabText);
}


bool ChatTabWidget::contains(const QString &userFullName) const
{
    return (getPrivateChat(userFullName) != nullptr);
}

ChatPanel *ChatTabWidget::getPrivateChat(const QString &userFullName) const
{
    return privateChats[userFullName];
}

void ChatTabWidget::setChatsTintColor(const QColor &color)
{
    if(mainChat)
        mainChat->setTintColor(color);

    for (auto chat : privateChats.values())
        chat->setTintColor(color);
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
