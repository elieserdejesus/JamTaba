#ifndef CHATPANEL_H
#define CHATPANEL_H

#include <QWidget>
#include <QMap>
#include <QPushButton>
#include <QList>
#include "chords/ChordProgression.h"
#include "UsersColorsPool.h"

namespace Ui {
class ChatPanel;
}

class ChatPanel : public QWidget
{
    Q_OBJECT

public:
    ChatPanel(const QStringList &botNames, UsersColorsPool *colorsPool);
    virtual ~ChatPanel();
    void addMessage(const QString &userName, const QString &userMessage, bool showTranslationButton = true, bool showBlockButton = false);
    void addBpmVoteConfirmationMessage(int newBpmValue);
    void addBpiVoteConfirmationMessage(int newBpmValue);
    void addChordProgressionConfirmationMessage(const ChordProgression &progression);
    void setPreferredTranslationLanguage(const QString &targetLanguage);
    void updateMessagesGeometry();// called when user switch from mini mode to full view
    void removeMessagesFrom(const QString &userName);
signals:
    void userSendingNewMessage(const QString &msg);
    void userConfirmingVoteToBpiChange(int newBpi);
    void userConfirmingVoteToBpmChange(int newBpm);
    void userConfirmingChordProgression(const ChordProgression &chordProgression);
    void userBlockingChatMessagesFrom(const QString &blockedUserName);
private slots:
    void on_chatTextEditionFinished();
    void on_verticalScrollBarRangeChanged(int min, int max);
    void on_buttonClear_clicked();

    void on_voteButtonClicked();
    void on_chordProgressionConfirmationButtonClicked();
    void on_buttonAutoTranslate_clicked();

    void showTranslationProgressFeedback();
    void hideTranslationProgressFeedback();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void changeEvent(QEvent *) override;

private:
    Ui::ChatPanel *ui;
    QColor getUserColor(const QString &userName);
    QStringList botNames;
    static const QColor BOT_COLOR;

    static const int MAX_MESSAGES = 50;

    QString autoTranslationLanguage;

    void createVoteButton(const QString &voteType, int value);

    bool autoTranslating;

    UsersColorsPool *colorsPool;
};

class NinjamVoteButton : public QPushButton
{
    Q_OBJECT

public:
    NinjamVoteButton(QString voteType, int voteValue) :
        voteValue(voteValue),
        voteType(voteType)
    {
        setObjectName("voteButton");

        QString label = tr("Vote - change %1 to %2 ").arg(voteType).arg(QString::number(voteValue));
        setText(label);
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
    int voteValue;
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
