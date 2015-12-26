#include "ChatPanel.h"
#include "ui_ChatPanel.h"
#include "ChatMessagePanel.h"
#include <QWidget>
#include <QScrollBar>
#include <QDebug>
#include <QKeyEvent>
#include <QWidget>

const QColor ChatPanel::BOT_COLOR(120, 120, 120);

ChatPanel::ChatPanel(QWidget *parent, QStringList botNames) :
    QWidget(parent),
    ui(new Ui::ChatPanel),
    botNames(botNames),
    availableColors(createColors()),
    autoTranslating(false)
{
    ui->setupUi(this);
    ui->scrollContent->setLayout(new QVBoxLayout(ui->scrollContent));
    ui->scrollContent->layout()->setContentsMargins(0, 0, 0, 0);

    QObject::connect(ui->chatText, SIGNAL(returnPressed()), this,
                     SLOT(on_chatTextEditionFinished()));
    ui->chatText->installEventFilter(this);

    // this event is used to auto scroll down when new messages are added
    QObject::connect(ui->chatScroll->verticalScrollBar(), SIGNAL(rangeChanged(int,
                                                                              int)), this,
                     SLOT(on_verticalScrollBarRangeChanged(int, int)));

    // disable blue border when QLineEdit has focus in mac
    ui->chatText->setAttribute(Qt::WA_MacShowFocusRect, 0);
}

bool ChatPanel::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->chatText && event->type() == QEvent::MouseButtonPress)
        ui->chatText->setFocus();
    return QWidget::eventFilter(obj, event);
}

// ++++++++++++++++++++++++++++++++
class NinjamVoteButton : public QPushButton
{
public:
    NinjamVoteButton(QString voteType, int voteValue) :
        QPushButton("Vote - change "+ voteType +" to " + QString::number(voteValue)),
        voteValue(voteValue),
        voteType(voteType)
    {
        setObjectName("voteButton");
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

void ChatPanel::createVoteButton(QString voteType, int value)
{
    QPushButton *voteButton = new NinjamVoteButton(voteType, value);
    voteButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
    ui->scrollContent->layout()->addWidget(voteButton);
    ui->scrollContent->layout()->setAlignment(voteButton, Qt::AlignRight);
    QObject::connect(voteButton, SIGNAL(clicked(bool)), this, SLOT(on_voteButtonClicked()));
}

void ChatPanel::addBpiVoteConfirmationMessage(int newBpiValue)
{
    createVoteButton("BPI", newBpiValue);
}

void ChatPanel::addBpmVoteConfirmationMessage(int newBpmValue)
{
    createVoteButton("BPM", newBpmValue);
}

void ChatPanel::on_voteButtonClicked()
{
    NinjamVoteButton *voteButton = static_cast<NinjamVoteButton *>(QObject::sender());
    if (voteButton->isBpiVote())
        emit userConfirmingVoteToBpiChange(voteButton->getVoteValue());
    else if (voteButton->isBpmVote())
        emit userConfirmingVoteToBpmChange(voteButton->getVoteValue());

    voteButton->parentWidget()->layout()->removeWidget(voteButton);
    voteButton->deleteLater();
}

// ++++++++++++++++++++++++++++++++++

class ChordProgressionConfirmationButton : public QPushButton
{
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

void ChatPanel::addChordProgressionConfirmationMessage(ChordProgression progression)
{
    QString buttonText = "Use/load the chords above";
    QPushButton *chordProgressionButton = new ChordProgressionConfirmationButton(buttonText,
                                                                                 progression);
    chordProgressionButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,
                                                      QSizePolicy::Preferred));
    ui->scrollContent->layout()->addWidget(chordProgressionButton);
    ui->scrollContent->layout()->setAlignment(chordProgressionButton, Qt::AlignRight);
    QObject::connect(chordProgressionButton, SIGNAL(clicked(bool)), this,
                     SLOT(on_chordProgressionConfirmationButtonClicked()));
}

// +++++++++++++++++++++++++++++++
void ChatPanel::on_chordProgressionConfirmationButtonClicked()
{
    ChordProgressionConfirmationButton *chordProgressionButton
        = static_cast<ChordProgressionConfirmationButton *>(QObject::sender());

    emit userConfirmingChordProgression(chordProgressionButton->getChordProgression());

    chordProgressionButton->parentWidget()->layout()->removeWidget(chordProgressionButton);
    chordProgressionButton->deleteLater();
}

// +++++++++++++++
void ChatPanel::on_verticalScrollBarRangeChanged(int min, int max)
{
    Q_UNUSED(min)
    // used to auto scroll down to keep the last added message visible
    ui->chatScroll->verticalScrollBar()->setValue(max + 10);
}

void ChatPanel::on_chatTextEditionFinished()
{
    if (!ui->chatText->text().isEmpty()) {
        emit userSendingNewMessage(ui->chatText->text());
        ui->chatText->clear();
    }
}

void ChatPanel::updateMessagesGeometry()
{
    QList<ChatMessagePanel *> messages = ui->scrollContent->findChildren<ChatMessagePanel *>();
    foreach (ChatMessagePanel *msg, messages) {
        msg->setMaximumWidth(ui->scrollContent->width());
        msg->updateGeometry();
    }
}

void ChatPanel::addMessage(QString userName, QString userMessage, bool showTranslationButton)
{
    QColor msgBackgroundColor = getUserColor(userName);
    QColor textColor = (msgBackgroundColor == BOT_COLOR) ? QColor(50, 50, 50) : QColor(0, 0, 0);
    QColor userNameBackgroundColor = msgBackgroundColor;
    if (msgBackgroundColor != BOT_COLOR)
        userNameBackgroundColor.setHsvF(msgBackgroundColor.hueF(),
                                        msgBackgroundColor.saturationF(),
                                        msgBackgroundColor.valueF() - 0.2);
    ChatMessagePanel *msgPanel = new ChatMessagePanel(ui->scrollContent, userName, userMessage,
                                                      userNameBackgroundColor, msgBackgroundColor,
                                                      textColor, showTranslationButton);
    msgPanel->setPrefferedTranslationLanguage(this->preferredTargetTranslationLanguage);
    msgPanel->setMaximumWidth(ui->scrollContent->width());
    ui->scrollContent->layout()->addWidget(msgPanel);
    if (ui->scrollContent->layout()->count() > MAX_MESSAGES) {
        // remove the first widget
        QList<ChatMessagePanel *> panels = ui->scrollContent->findChildren<ChatMessagePanel *>();
        ui->scrollContent->layout()->removeWidget(panels.first());
        delete panels.first();
    }
    ui->scrollContent->layout()->setAlignment(Qt::AlignTop);
    ui->scrollContent->layout()->setAlignment(msgPanel, Qt::AlignTop);

    if (autoTranslating)
        msgPanel->translate();// request the translation

}

// +++++++++++++++++++++++++++++++++++=
QList<QColor> ChatPanel::createColors()
{
    QList<QColor> colors;
    const int totalColors = 8;
    for (int x = 0; x < totalColors; ++x) {
        QColor color;
        qreal h = (qreal)x/totalColors;
        color.setHsvF(h, 0.25, 0.95);
        colors.append(color);
    }
    return colors;
}

QColor ChatPanel::getUserColor(QString userName)
{
    if (botNames.contains(userName) || userName.isNull() || userName.isEmpty()
        || userName == "Jamtaba")
        return BOT_COLOR;

    QColor color;
    if (!usersColorMap.contains(userName)) {
        color = availableColors.at(0);
        availableColors.removeFirst();// remove do início
        availableColors.append(color);// e coloca no fim, caso tenha muitos usuários a cor será usada novamente
        usersColorMap.insert(userName, color);
    }
    return usersColorMap[userName];
}

ChatPanel::~ChatPanel()
{
    delete ui;
}

void ChatPanel::on_buttonClear_clicked()
{
    // remove message panels
    QList<ChatMessagePanel *> panels = ui->scrollContent->findChildren<ChatMessagePanel *>();
    foreach (ChatMessagePanel *msgPanel, panels) {
        ui->scrollContent->layout()->removeWidget(msgPanel);
        msgPanel->deleteLater();
    }

    // remove Vote buttons
    QList<NinjamVoteButton *> buttons = ui->scrollContent->findChildren<NinjamVoteButton *>();
    foreach (NinjamVoteButton *button, buttons) {
        ui->scrollContent->layout()->removeWidget(button);
        button->deleteLater();
    }
}

void ChatPanel::setPreferredTranslationLanguage(QString targetLanguage)
{
    if (targetLanguage != this->preferredTargetTranslationLanguage) {
        this->preferredTargetTranslationLanguage = targetLanguage;
        QList<ChatMessagePanel *> panels = ui->scrollContent->findChildren<ChatMessagePanel *>();
        foreach (ChatMessagePanel *msgPanel, panels)
            msgPanel->setPrefferedTranslationLanguage(targetLanguage);
    }
}

void ChatPanel::on_buttonAutoTranslate_clicked()
{
    this->autoTranslating = ui->buttonAutoTranslate->isChecked();
}
