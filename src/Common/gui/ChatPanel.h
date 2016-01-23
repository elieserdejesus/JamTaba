#ifndef CHATPANEL_H
#define CHATPANEL_H

#include <QWidget>
#include <QMap>
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
    void addMessage(const QString &userName, const QString &userMessage, bool showTranslationButton = true);
    void addBpmVoteConfirmationMessage(int newBpmValue);
    void addBpiVoteConfirmationMessage(int newBpmValue);
    void addChordProgressionConfirmationMessage(const ChordProgression &progression);
    void setPreferredTranslationLanguage(const QString &targetLanguage);
    void updateMessagesGeometry();// called when user switch from mini mode to full view
signals:
    void userSendingNewMessage(const QString &msg);
    void userConfirmingVoteToBpiChange(int newBpi);
    void userConfirmingVoteToBpmChange(int newBpm);
    void userConfirmingChordProgression(const ChordProgression &chordProgression);
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

private:
    Ui::ChatPanel *ui;
    QColor getUserColor(const QString &userName);
    QStringList botNames;
    static const QColor BOT_COLOR;

    static const int MAX_MESSAGES = 50;

    QString preferredTargetTranslationLanguage;

    void createVoteButton(const QString &voteType, int value);

    bool autoTranslating;

    UsersColorsPool *colorsPool;
};

#endif // CHATPANEL_H
