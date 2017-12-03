#ifndef CHATTABWIDGET_H
#define CHATTABWIDGET_H

#include <QFrame>
#include <QTabBar>
#include <QStackedWidget>

class UsersColorsPool;
class ChatPanel;
class TextEditorModifier;

class ChatTabWidget : public QFrame
{
    Q_OBJECT

public:
    ChatTabWidget(QWidget *parent, const QStringList &botNames, UsersColorsPool *colorsPool);

    void collapse(bool collapse);

    void toggleCollapse();

    void setPreferredTranslationLanguage(const QString locale);

    ChatPanel* getMainChat() const;

    ChatPanel *getFocusedChatPanel() const;

    QList<ChatPanel *> getChats() const;

    bool contains(const QString &userFullName) const;

    ChatPanel *getPrivateChat(const QString &userFullName);

    ChatPanel *createPublicChat(const QString &jamTabaChatBotName, const QString &preferredLanguage, TextEditorModifier *textEditorModifier);
    ChatPanel *createPrivateChat(const QString &remoteUserName, const QString &userIP, TextEditorModifier *textModifider, bool focusNewChat);

    void updatePublicChatTabTitle(uint unreadedMessages = 0);

    bool isCollapsed() const;

    void clear();

signals:
    void collapsedChanged(bool collapsed);

private slots:
    void closeChatTab(int index);

private:
    QTabBar *tabBar;
    QStackedWidget *stackWidget;

    QStringList botNames;
    UsersColorsPool *colorsPool;

    ChatPanel *mainChat;

    void updatePrivateChatTabTitle(int chatIndex, uint unreadedMessages);

    void removeTabCloseButton(int buttonIndex);
};


inline ChatPanel *ChatTabWidget::getMainChat() const
{
    Q_ASSERT(mainChat);

    return mainChat;
}

#endif // CHATTABWIDGET_H
