#include "ChatPanel.h"
#include "ui_ChatPanel.h"

#include "ChatMessagePanel.h"
#include <QWidget>
#include <QScrollBar>
#include <QDebug>

const QColor ChatPanel::BOT_COLOR(120, 120, 120);

ChatPanel::ChatPanel(QWidget *parent, QStringList botNames) :
    QWidget(parent),
    ui(new Ui::ChatPanel),
    botNames(botNames),
    availableColors(createColors())
{
    ui->setupUi(this);
    ui->scrollContent->setLayout(new QVBoxLayout(ui->scrollContent));
    ui->scrollContent->layout()->setContentsMargins(0, 0, 0, 0);

    QObject::connect(ui->chatText, SIGNAL(returnPressed()), this, SLOT(on_chatTextEditionFinished()));

    //this event is used to auto scroll down when new messages are added
    QObject::connect(ui->chatScroll->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(on_verticalScrollBarRangeChanged(int,int)));

    //disable blue border when QLineEdit has focus in mac
    ui->chatText->setAttribute(Qt::WA_MacShowFocusRect, 0);

    //create some messages to test
    /*
    for (int user = 0; user < 11; ++user) {
        for (int c = 0; c < 1; ++c) {
            QString userName = "User " + QString::number(user);
            QString message = "Message masdasd masd amsd amsd masd masd masd masd masd masd masd masd masd masd amsda";
            if(user % 2 == 0){
                userName = userName + " test";
                message = "short message";
            }
            addMessage(userName, message);
        }
    }
    */

    //QString teste("Ã©");
    //addMessage("elieser", QString::fromUtf8(teste.toStdString().c_str()));
}

void ChatPanel::on_verticalScrollBarRangeChanged(int min, int max){
    Q_UNUSED(min)
    //used to auto scroll down to keep the last added message visible
    ui->chatScroll->verticalScrollBar()->setValue(max + 10);
}

void ChatPanel::on_chatTextEditionFinished(){
    if(!ui->chatText->text().isEmpty()){
        emit userSendingNewMessage(ui->chatText->text());
        ui->chatText->clear();
    }
}

void ChatPanel::addMessage(QString userName, QString userMessage){
    QColor msgBackgroundColor = getUserColor(userName);
    QColor textColor = msgBackgroundColor == BOT_COLOR ? QColor(50, 50, 50) : QColor(0, 0, 0);
    QColor userNameBackgroundColor = msgBackgroundColor;
    bool drawBorder = true;
    if(msgBackgroundColor != BOT_COLOR){
        userNameBackgroundColor.setHsvF(msgBackgroundColor.hueF(), msgBackgroundColor.saturationF(), msgBackgroundColor.valueF() - 0.2);
    }
    else{
        drawBorder = false;
    }
    ChatMessagePanel* msgPanel = new ChatMessagePanel(ui->scrollContent, userName, userMessage, userNameBackgroundColor, msgBackgroundColor, textColor, drawBorder);
    ui->scrollContent->layout()->addWidget(msgPanel);
    if(ui->scrollContent->layout()->count() > MAX_MESSAGES){
        //remove the first widget
        QList<ChatMessagePanel*> panels = ui->scrollContent->findChildren<ChatMessagePanel*>();
        ui->scrollContent->layout()->removeWidget(panels.first());
        delete panels.first();
    }
    ui->scrollContent->layout()->setAlignment(Qt::AlignTop);
}

QList<QColor> ChatPanel::createColors(){
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

QColor ChatPanel::getUserColor(QString userName) {
    if ( botNames.contains(userName) || userName.isNull() || userName.isEmpty() ) {
        return BOT_COLOR;
    }

    QColor color;
    if (!usersColorMap.contains(userName)) {
        color = availableColors.at(0);
        availableColors.removeFirst();//remove do início
        availableColors.append(color);//e coloca no fim, caso tenha muitos usuários a cor será usada novamente
        usersColorMap.insert(userName, color);
    }
    return usersColorMap[userName];
}

ChatPanel::~ChatPanel(){
    delete ui;
}

void ChatPanel::on_buttonClear_clicked(){
    QList<ChatMessagePanel*> panels = ui->scrollContent->findChildren<ChatMessagePanel*>();
    foreach (ChatMessagePanel* msgPanel, panels) {
        ui->scrollContent->layout()->removeWidget(msgPanel);
        delete msgPanel;
    }

}
