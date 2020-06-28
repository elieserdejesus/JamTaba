#ifndef CHATPANEL_H
#define CHATPANEL_H

#include <QWidget>
#include <QMap>
#include <QPushButton>
#include <QList>
#include <QTimer>
#include <QAction>
#include <QTreeWidgetItem>

#include "gui/chords/ChordProgression.h"

namespace Ui {
class ChatPanel;
}

namespace login {
struct Location;
}

class ChatMessagePanel;
class EmojiWidget;
class EmojiManager;
class TextEditorModifier;
class UsersColorsPool;

class ChatPanel : public QWidget
{
    Q_OBJECT

public:
    ChatPanel(const QStringList &botNames, UsersColorsPool *colorsPool, TextEditorModifier *chatInputModifier, EmojiManager *emojiManager);

    virtual ~ChatPanel();

    void addMessage(const QString &localUserName, const QString &msgAuthorFullName, const QString &msgText, bool showTranslationButton = true, bool showBlockButton = false);
    void addLastChordsMessage(const QString &userName, const QString &message, QColor textColor = Qt::black, QColor backgroundColor = QColor(212, 243, 182));
    void addBpmVoteConfirmationMessage(quint32 newBpmValue, quint32 expireTime);
    void addBpiVoteConfirmationMessage(quint32 newBpiValue, quint32 expireTime);
    void addChordProgressionConfirmationMessage(const ChordProgression &progression);
    void setPreferredTranslationLanguage(const QString &targetLanguage);
    void updateMessagesGeometry(); // called when user switch from mini mode to full view
    void removeMessagesFrom(const QString &userFullName);
    void setInputsStatus(bool enabled);
    bool inputsAreEnabled() const;

    void setConnectedUsers(const QStringList &usersNames);
    QList<QString> getConnectedUsers() const;

    void setConnectedUserBlockedStatus(const QString &usersFullName, bool blocked);

    void setTintColor(const QColor &color);

    void setRemoteUserFullName(const QString &remoteUserFullName);
    QString getRemoteUserFullName() const;

    void showConnectedUsersWidget(bool show);

    void hideOnOffButton();

    void turnOn();
    void turnOff();

    bool isOn() const;

    static void setFontSizeOffset(qint8 sizeOffset);

public slots:
    void setTopicMessage(const QString &topic);
    void updateUsersLocation(const QString &ip, const login::Location &location);
    void createServerInviteButton(const QString &serverIP, quint16 serverPort);

signals:
    void userSendingNewMessage(const QString &msg);
    void userConfirmingVoteToBpiChange(int newBpi);
    void userConfirmingVoteToBpmChange(int newBpm);
    void userConfirmingChordProgression(const ChordProgression &chordProgression);
    void userBlockingChatMessagesFrom(const QString &blockedUserName);
    void userUnblockingChatMessagesFrom(const QString &blockedUserName);
    void userAcceptingServerInvite(const QString &serverIP, quint16 serverPort);
    void unreadedMessagesChanged(int unreadedMessages);
    void fontSizeOffsetEdited(qint8 newOffset); // font size offset edited by user
    void connectedUserContextMenuActivated(QMenu &menu, const QString &userFullName);
    void turnedOn();
    void turnedOff();

private slots:
    void sendNewMessage();
    void autoScroll(int min, int max);
    void clearMessages();

    void confirmVote();
    void confirmChordProgression();
    void toggleAutoTranslate();

    void showTranslationProgressFeedback();
    void hideTranslationProgressFeedback();

    void increaseFontSize();
    void decreaseFontSize();

    void showContextMenu(const QPoint &pos);

    void toggleOnOff();

protected:
    void changeEvent(QEvent *) override;
    void showEvent(QShowEvent *) override;
    bool eventFilter(QObject *, QEvent *) override;

private:
    Ui::ChatPanel *ui;
    QStringList botNames;
    static const QColor BOT_COLOR;

    EmojiWidget *emojiWidget;
    EmojiManager *emojiManager;
    QAction *emojiAction;

    QString remoteUserFulName; // used in private chats only

    static const int MAX_MESSAGES = 50;

    int previousVerticalScrollBarMaxValue;

    QString autoTranslationLanguage;

    bool autoTranslating;

    UsersColorsPool *colorsPool;

    uint unreadedMessages; // messages added when this widget is not focused

    bool on;

    QColor tintColor;

    static qint8 fontSizeOffset;

    static QList<ChatPanel *> instances;

    const static qint8 MAX_FONT_OFFSET;
    const static qint8 MIN_FONT_OFFSET;

    static void setItemCountryDetails(QTreeWidgetItem *item, const login::Location &location);

    QColor getUserColor(const QString &userName);

    void addMessagePanelInLayout(ChatMessagePanel *msgPanel, Qt::Alignment alignment);

    void createVoteButton(const QString &voteType, quint32 value, quint32 expireTime);

    void setUnreadedMessages(uint unreaded);

    void setupSignals();

    void setMessagesFontSizeOffset(qint8 offset);

    void updatePlaceHolderText();
    void updateConnectedUsersTitleString(quint32 connectedUsers);
    void updateEmojiIcon();
};

inline QString ChatPanel::getRemoteUserFullName() const
{
    return remoteUserFulName;
}

inline void ChatPanel::setRemoteUserFullName(const QString &remoteUserFullName)
{
    this->remoteUserFulName = remoteUserFullName;
}

class ServerInviteButton : public QPushButton
{
    Q_OBJECT

public:
    ServerInviteButton(const QString &serverIP, quint16 serverPort, QWidget *parent = nullptr) :
        QPushButton(tr("Accept!"), parent),
        serverIP(serverIP),
        serverPort(serverPort)
    {
        //
    }

    QString getServerIP() const;
    quint16 getServerPort() const;

private:
    QString serverIP;
    quint16 serverPort;
};

inline quint16 ServerInviteButton::getServerPort() const
{
    return serverPort;
}

inline QString ServerInviteButton::getServerIP() const
{
    return serverIP;
}

class NinjamVoteButton : public QPushButton
{
    Q_OBJECT

public:
    NinjamVoteButton(QString voteType, quint32 voteValue, quint32 expireTime) :
        voteValue(voteValue),
        voteType(voteType)
    {
        setObjectName("voteButton");

        QString label = tr("Vote - change %1 to %2 ").arg(voteType).arg(QString::number(voteValue));
        setText(label);

        QTimer::singleShot(expireTime * 1000, this, SLOT(hide()));
    }

    inline int getVoteValue() const
    {
        return voteValue;
    }

    inline bool isBpiVote() const
    {
        return voteType == "BPI";
    }

    inline bool isBpmVote() const
    {
        return voteType == "BPM";
    }

private:
    quint32 voteValue;
    QString voteType;
};



class ChordProgressionConfirmationButton : public QPushButton
{
    Q_OBJECT

public:
    ChordProgressionConfirmationButton(QString text, ChordProgression progression) :
        QPushButton(text),
        progression(progression)
    {
    }

    inline ChordProgression getChordProgression() const
    {
        return progression;
    }

private:
    ChordProgression progression;
};

#endif // CHATPANEL_H
