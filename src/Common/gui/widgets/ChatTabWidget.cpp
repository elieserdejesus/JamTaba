#include "ChatTabWidget.h"

#include "MainController.h"

#include <QBoxLayout>
#include <QStyle>
#include <QRegularExpression>
#include <QPainter>
#include <QFont>

#include "gui/chat/ChatPanel.h"
#include "gui/UsersColorsPool.h"
#include "ninjam/client/User.h"

using controller::MainController;

ChatTabWidget::ChatTabWidget(QWidget *parent) :
    QFrame(parent),
    tabBar(new QTabBar(this)),
    stackWidget(new QStackedWidget(this)),
    mainChat(nullptr),
    ninjamServerChat(nullptr),
    privateChatColorsPool(new UsersColorsPool()),
    mainChatColorsPool(new UsersColorsPool())
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
    stackWidget->setMaximumWidth(250);
}

void ChatTabWidget::initialize(MainController *mainController, UsersColorsPool *colorsPool)
{
    this->ninjamColorsPool = colorsPool;
    this->mainController = mainController;

    connect(mainController, &MainController::ipResolved, [=](const QString &ip){
        mainChat->updateUsersLocation(ip, mainController->getGeoLocation(ip));
    });
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

    ninjamServerChat = nullptr;

    for (auto chat : privateChats.values())
        chat->deleteLater();

    privateChats.clear();
}

void ChatTabWidget::retranslateUi()
{
    tabBar->setTabText(0, tr("Chat"));
    if (mainChat)
        mainChat->setTopicMessage(tr("Public chat"));
}

void ChatTabWidget::closeNinjamChats()
{
    while (tabBar->count() > 1) {
        closeChatTab(1);
    }

    privateChats.clear();
    ninjamServerChat = nullptr;
}

ChatPanel *ChatTabWidget::createMainChat(TextEditorModifier *textEditorModifier)
{
    if (mainChat) {
        return mainChat;
    }

    tabBar->addTab(tr("Chat")); // add main chat

    auto botNames = mainController->getBotNames();
    auto emojiManager = mainController->getEmojiManager();
    mainChatColorsPool->giveBackAllColors();
    mainChat = new ChatPanel(botNames, mainChatColorsPool, textEditorModifier, emojiManager);
    stackWidget->addWidget(mainChat);


    mainChat->setTopicMessage(tr("Public chat"));

    connect(mainChat, &ChatPanel::unreadedMessagesChanged, this, [=](uint unreaded) {

        updateMainChatTabTitle(unreaded);
    });

    removeTabCloseButton(0); // the main chat is not closable

    updateMainChatTabTitle(); // set and translate the main tab title

    mainChat->setPreferredTranslationLanguage(mainController->getTranslationLanguage());

    return mainChat;

}

ChatPanel *ChatTabWidget::createNinjamServerChat(const QString &serverName, TextEditorModifier *textEditorModifier)
{
    Q_ASSERT(!ninjamServerChat);

    // add ninjam main chat
    auto index = tabBar->addTab("Ninjam Chat");

    auto botNames = mainController->getBotNames();
    auto emojiManager = mainController->getEmojiManager();
    ninjamColorsPool->giveBackAllColors();
    ninjamServerChat = new ChatPanel(botNames, ninjamColorsPool, textEditorModifier, emojiManager);
    stackWidget->addWidget(ninjamServerChat);

    connect(ninjamServerChat, &ChatPanel::unreadedMessagesChanged, this, [=](uint unreaded) {

        updateNinjamChatTabTitle(unreaded);
    });

    removeTabCloseButton(1); // the ninjam server chat is not closable

    ninjamServerChat->setPreferredTranslationLanguage(mainController->getTranslationLanguage());

    tabBar->setCurrentIndex(index); // auto change the chat focus no ninjam chat when enter in a server
    tabBar->setToolTip(serverName);

    return ninjamServerChat;
}

void ChatTabWidget::setConnectedUsersInMainChat(const QStringList &usersNames)
{
    Q_ASSERT(mainChat);

    mainChat->setConnectedUsers(usersNames);

    for (const QString &userFullName : usersNames) {
        auto ip = ninjam::client::extractUserIP(userFullName);
        mainChat->updateUsersLocation(ip, mainController->getGeoLocation(ip));
    }
}

QIcon ChatTabWidget::createChatTabIcon(uint unreadedMessages)
{
    if (unreadedMessages <= 0)
        return QIcon();

    static const qreal SIZE = 16.0;

    static const QSizeF size(SIZE, SIZE);

    QPixmap pixmap(size.toSize());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    QString text(QString::number(unreadedMessages));

    auto font = QFont("Arial", 7, QFont::Normal);

    QRectF textRect(QPointF(), size);

    painter.setBrush(QColor(255, 0, 0, 180));
    painter.setPen(Qt::black);
    painter.drawEllipse(QRectF(QPointF(1, 1), QSizeF(SIZE - 2, SIZE - 2)));

    painter.setPen(Qt::white);
    painter.setFont(font);
    painter.drawText(textRect, text, QTextOption(Qt::AlignCenter));

    return QIcon(pixmap);
}

void ChatTabWidget::updateNinjamChatTabTitle(uint unreadedMessages)
{
    int chatTabIndex = 1; // assuming ninjam chat is always the second tab
    tabBar->setTabIcon(chatTabIndex, createChatTabIcon(unreadedMessages));
}

void ChatTabWidget::updateMainChatTabTitle(uint unreadedMessages)
{
    int chatTabIndex = 0; // assuming main chat is always the first tab
    tabBar->setTabIcon(chatTabIndex, createChatTabIcon(unreadedMessages));
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
        privateChatColorsPool->giveBackAllColors();
        chatPanel = new ChatPanel(botNames, privateChatColorsPool, textModifider, emojiManager);
        chatPanel->setRemoteUserFullName(remoteUserFullName);
        int tabIndex = tabBar->addTab(remoteUserName);
        stackWidget->addWidget(chatPanel);

        connect(chatPanel, &ChatPanel::unreadedMessagesChanged, this, [=](uint unreaded) {
            updatePrivateChatTabTitle(tabIndex, unreaded);
        });

        if (focusNewChat)
            tabBar->setCurrentIndex(tabIndex);

        privateChats.insert(remoteUserFullName, chatPanel);
    }

    return chatPanel;
}

void ChatTabWidget::updatePrivateChatTabTitle(int chatIndex, uint unreadedMessages)
{
    Q_ASSERT(chatIndex > 1); // index 0 is the public chat, index 1 is the ninjam server chat

    auto chatPanel = static_cast<ChatPanel *>(stackWidget->widget(chatIndex));
    if (!chatPanel)
        return;

    tabBar->setTabIcon(chatIndex, createChatTabIcon(unreadedMessages));
}


bool ChatTabWidget::contains(const QString &userFullName) const
{
    return (getPrivateChat(userFullName) != nullptr);
}

ChatPanel *ChatTabWidget::getPublicChat() const
{
    return mainChat;
}

ChatPanel *ChatTabWidget::getPrivateChat(const QString &userFullName) const
{
    return privateChats[userFullName];
}

void ChatTabWidget::setChatsTintColor(const QColor &color)
{
    if(ninjamServerChat)
        ninjamServerChat->setTintColor(color);

    for (auto chat : privateChats.values())
        chat->setTintColor(color);

    if (mainChat)
        mainChat->setTintColor(color);
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
