#ifndef CHATPANEL_H
#define CHATPANEL_H

#include <QWidget>
#include <QMap>
#include <QPushButton>
#include <QList>
#include <QTimer>
#include <QAction>

#include "gui/chords/ChordProgression.h"

namespace Ui {
class ChatPanel;
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

    void addMessage(const QString &localUserName, const QString &msgAuthor, const QString &msgText, bool showTranslationButton = true, bool showBlockButton = false);
    void addLastChordsMessage(const QString &userName, const QString &message, QColor textColor = Qt::black, QColor backgroundColor = QColor(212, 243, 182));
    void addBpmVoteConfirmationMessage(quint32 newBpmValue, quint32 expireTime);
    void addBpiVoteConfirmationMessage(quint32 newBpiValue, quint32 expireTime);
    void addChordProgressionConfirmationMessage(const ChordProgression &progression);
    void setPreferredTranslationLanguage(const QString &targetLanguage);
    void updateMessagesGeometry(); // called when user switch from mini mode to full view
    void removeMessagesFrom(const QString &userName);
    void setInputsStatus(bool enabled);
    bool inputsAreEnabled() const;

    void setConnectedUsers(const QStringList &usersNames);

    void setTintColor(const QColor &color);

    void setRemoteUserFullName(const QString &remoteUserFullName);
    QString getRemoteUserFullName() const;

    static void setFontSizeOffset(qint8 sizeOffset);

public slots:
    void setTopicMessage(const QString &topic);

signals:
    void userSendingNewMessage(const QString &msg);
    void userConfirmingVoteToBpiChange(int newBpi);
    void userConfirmingVoteToBpmChange(int newBpm);
    void userConfirmingChordProgression(const ChordProgression &chordProgression);
    void userBlockingChatMessagesFrom(const QString &blockedUserName);
    void unreadedMessagesChanged(int unreadedMessages);
    void fontSizeOffsetEdited(qint8 newOffset); // font size offset edited by user

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

    static qint8 fontSizeOffset;

    static QList<ChatPanel *> instances;

    const static qint8 MAX_FONT_OFFSET;
    const static qint8 MIN_FONT_OFFSET;

    QColor getUserColor(const QString &userName);

    void addMessagePanelInLayout(ChatMessagePanel *msgPanel, Qt::Alignment alignment);

    void createVoteButton(const QString &voteType, quint32 value, quint32 expireTime);

    void setUnreadedMessages(uint unreaded);

    void setupSignals();

    void setMessagesFontSizeOffset(qint8 offset);
};

inline QString ChatPanel::getRemoteUserFullName() const
{
    return remoteUserFulName;
}

inline void ChatPanel::setRemoteUserFullName(const QString &remoteUserFullName)
{
    this->remoteUserFulName = remoteUserFullName;
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
