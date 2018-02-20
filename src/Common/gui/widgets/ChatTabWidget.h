#ifndef CHATTABWIDGET_H
#define CHATTABWIDGET_H

#include <QFrame>
#include <QTabBar>
#include <QStackedWidget>
#include <QMap>

class UsersColorsPool;
class ChatPanel;
class TextEditorModifier;

namespace controller {
    class MainController;
}

class ChatTabWidget : public QFrame
{
    Q_OBJECT

public:
    ChatTabWidget(QWidget *parent);
    void initialize(controller::MainController *mainController, UsersColorsPool *colorsPool);

    void collapse(bool collapse);

    void toggleCollapse();

    void setPreferredTranslationLanguage(const QString locale);

    ChatPanel* getNinjamServerChat() const;

    ChatPanel *getFocusedChatPanel() const;

    bool contains(const QString &userFullName) const;

    ChatPanel *getPrivateChat(const QString &userFullName) const;

    ChatPanel *createMainChat(const QString &chatName, TextEditorModifier *textEditorModifier);
    ChatPanel *createNinjamServerChat(const QString &serverName, TextEditorModifier *textEditorModifier);
    ChatPanel *createPrivateChat(const QString &remoteUserName, const QString &userIP, TextEditorModifier *textModifider, bool focusNewChat);

    void updateNinjamChatTabTitle(uint unreadedMessages = 0);
    void updateMainChatTabTitle(uint unreadedMessages = 0);

    bool isCollapsed() const;

    void clear();

    void setChatsTintColor(const QColor &color);

signals:
    void collapsedChanged(bool collapsed);

private slots:
    void closeChatTab(int index);
    void changeCurrentTab(int tabIndex);

private:
    QTabBar *tabBar;
    QStackedWidget *stackWidget;

    UsersColorsPool *colorsPool;

    ChatPanel *ninjamServerChat;
    ChatPanel *mainChat;
    QMap<QString, ChatPanel*> privateChats;

    controller::MainController *mainController;

    void updatePrivateChatTabTitle(int chatIndex, uint unreadedMessages, const QString &remoteUserName);

    void removeTabCloseButton(int buttonIndex);

    static QIcon createChatTabIcon(uint unreadedMessages);
};


inline ChatPanel *ChatTabWidget::getNinjamServerChat() const
{
    Q_ASSERT(ninjamServerChat);

    return ninjamServerChat;
}

#endif // CHATTABWIDGET_H
