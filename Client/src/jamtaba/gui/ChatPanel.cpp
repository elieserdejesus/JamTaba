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

    QObject::connect(ui->chatText, SIGNAL(returnPressed()), this, SLOT(chatTextEditionFinished()));

    //this event is used to auto scroll down when new messages are added
    QObject::connect(ui->chatScroll->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(verticalScrollBarRangeChanged(int,int)));

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

void ChatPanel::verticalScrollBarRangeChanged(int min, int max){
    //used to auto scroll down to keep the last added message visible
    ui->chatScroll->verticalScrollBar()->setValue(max + 10);
}

void ChatPanel::chatTextEditionFinished(){
    if(!ui->chatText->text().isEmpty()){
        emit userSendingNewMessage(ui->chatText->text());
        ui->chatText->clear();
    }
}

void ChatPanel::addMessage(QString userName, QString userMessage){
    QColor bgColor = getUserColor(userName);
    ChatMessagePanel* msgPanel = new ChatMessagePanel(ui->scrollContent, userName, userMessage, bgColor);
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
    if ( botNames.contains(userName) ) {
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

ChatPanel::~ChatPanel()
{
    delete ui;
}
