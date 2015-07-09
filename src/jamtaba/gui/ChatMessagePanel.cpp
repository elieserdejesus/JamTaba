#include "ChatMessagePanel.h"
#include "ui_ChatMessagePanel.h"
#include <QDebug>

ChatMessagePanel::ChatMessagePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatMessagePanel)
{
    ui->setupUi(this);
}


ChatMessagePanel::ChatMessagePanel(QWidget *parent, QString userName, QString msg, QColor userNameBackgroundColor, QColor msgBackgroundColor, QColor textColor, bool drawBorder)
    :QWidget(parent),
      ui(new Ui::ChatMessagePanel)
{
    ui->setupUi(this);
    initialize(userName, msg, userNameBackgroundColor, msgBackgroundColor, textColor, drawBorder);
}

void ChatMessagePanel::initialize(QString userName, QString msg, QColor userNameBackgroundColor, QColor msgBackgroundColor, QColor textColor, bool drawBorder){
    if(!userName.isEmpty() && !userName.isNull()){
        ui->labelUserName->setText(userName);
        ui->labelUserName->setStyleSheet(buildCssString(userNameBackgroundColor, textColor, drawBorder));
    }
    else{
        ui->labelUserName->setVisible(false);
    }

    msg = msg.replace(QRegExp("<.+?>"), "");//scape html tags
    msg = msg.replace("\n", "<br/>");
    msg = replaceLinksInString(msg);
    ui->labelMessage->setText(msg);
    ui->labelMessage->setStyleSheet(buildCssString(msgBackgroundColor, textColor, drawBorder));
}

QString ChatMessagePanel::buildCssString(QColor bgColor, QColor textColor, bool drawBorder){
    QString css =   "background-color: " + colorToCSS(bgColor) + ";";
    css +=          "color: " + colorToCSS(textColor) + ";";
    if(!drawBorder){
        css +=          "border: none; ";
    }
    return css;
}

QString ChatMessagePanel::replaceLinksInString(QString str){
    QString regex = "((?:https?|ftp|www)://\\S+)";
    return str.replace(QRegExp(regex), "<a href=\"\\1\">\\1</a>");
}

QString ChatMessagePanel::colorToCSS(QColor color){
    return "rgb(" + QString::number(color.red()) + ", " + QString::number(color.green()) + ", " + QString::number(color.blue()) + ")";
}

ChatMessagePanel::~ChatMessagePanel()
{
    delete ui;
}
