#ifndef CHATPANEL_H
#define CHATPANEL_H

#include <QWidget>
#include <QMap>
#include <QList>


namespace Ui {
class ChatPanel;
}

class ChatPanel : public QWidget
{
    Q_OBJECT

public:
    ChatPanel(QWidget *parent, QStringList botNames);
    virtual ~ChatPanel();
    void addMessage(QString userName, QString userMessage);
    void addBpmVoteConfirmationMessage(int newBpmValue);
    void addBpiVoteConfirmationMessage(int newBpmValue);
signals:
    void userSendingNewMessage(QString msg);
    void userConfirmingVoteToBpiChange(int newBpi);
    void userConfirmingVoteToBpmChange(int newBpm);
private slots:
    void on_chatTextEditionFinished();
    void on_verticalScrollBarRangeChanged(int min, int max);
    void on_buttonClear_clicked();

    void on_voteButtonClicked();

private:
    Ui::ChatPanel *ui;
    QColor getUserColor(QString userName);
    QStringList botNames;
    static const QColor BOT_COLOR;
    QMap<QString, QColor> usersColorMap;//map user name to a color
    QList<QColor> availableColors;

    QList<QColor> createColors();

    static const int MAX_MESSAGES = 50;

    void createVoteButton(QString voteType, int value);

    //QString lastUserName;//used to control message alignment - left or right
    //Qt::AlignmentFlag lastAlign;
};

#endif // CHATPANEL_H
