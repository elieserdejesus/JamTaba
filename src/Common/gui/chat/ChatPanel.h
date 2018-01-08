#ifndef CHATPANEL_H
#define CHATPANEL_H

#include <QWidget>
#include <QMap>
#include <QPushButton>
#include <QList>
#include <QTimer>
#include <QAction>

#include "chords/ChordProgression.h"
#include "UsersColorsPool.h"
#include "TextEditorModifier.h"
#include "EmojiManager.h"

namespace Ui {
class ChatPanel;
}

class ChatMessagePanel;
class EmojiWidget;

namespace Controller {
class MainController;
}

class ChatPanel : public QWidget
{
    Q_OBJECT

public:
    ChatPanel(const QString &userFullName, Controller::MainController *mainController, UsersColorsPool *colorsPool, TextEditorModifier *chatInputModifier);

    virtual ~ChatPanel();

    void addMessage(const QString &userName, const QString &userMessage, bool showTranslationButton = true, bool showBlockButton = false);
    void addLastChordsMessage(const QString &userName, const QString &message, QColor textColor = Qt::black, QColor backgroundColor = QColor(212, 243, 182));
    void addBpmVoteConfirmationMessage(quint32 newBpmValue, quint32 expireTime);
    void addBpiVoteConfirmationMessage(quint32 newBpiValue, quint32 expireTime);
    void addChordProgressionConfirmationMessage(const ChordProgression &progression);
    void setPreferredTranslationLanguage(const QString &targetLanguage);
    void updateMessagesGeometry(); // called when user switch from mini mode to full view
    void removeMessagesFrom(const QString &userName);
    void setInputsStatus(bool enabled);
    bool inputsAreEnabled() const;
    QString getUserFullName() const;

    void setTintColor(const QColor &color);

public slots:
    void setTopicMessage(const QString &topic);

signals:
    void userSendingNewMessage(const QString &msg);
    void userConfirmingVoteToBpiChange(int newBpi);
    void userConfirmingVoteToBpmChange(int newBpm);
    void userConfirmingChordProgression(const ChordProgression &chordProgression);
    void userBlockingChatMessagesFrom(const QString &blockedUserName);
    void unreadedMessagesChanged(int unreadedMessages);

private slots:
    void sendNewMessage();
    void autoScroll(int min, int max);
    void clearMessages();

    void confirmVote();
    void confirmChordProgression();
    void toggleAutoTranslate();

    void showTranslationProgressFeedback();
    void hideTranslationProgressFeedback();

protected:
    void changeEvent(QEvent *) override;
    void showEvent(QShowEvent *) override;

private:
    Ui::ChatPanel *ui;
    QColor getUserColor(const QString &userName);
    QStringList botNames;
    static const QColor BOT_COLOR;

    EmojiManager emojiManager;
    EmojiWidget *emojiWidget;
    Controller::MainController *mainController;

    QAction *emojiAction;
    QColor tintColor;

    static const int MAX_MESSAGES = 50;

    int previousVerticalScrollBarMaxValue;

    QString autoTranslationLanguage;

    void createVoteButton(const QString &voteType, quint32 value, quint32 expireTime);

    bool autoTranslating;

    void addMessagePanelInLayout(ChatMessagePanel *msgPanel);

    UsersColorsPool *colorsPool;

    QString userFullName; // username@ip

    uint unreadedMessages; // messages added when this widget is not focused

    void setUnreadedMessages(uint unreaded);
};

inline QString ChatPanel::getUserFullName() const
{
    return userFullName;
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
