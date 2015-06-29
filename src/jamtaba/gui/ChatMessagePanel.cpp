#include "ChatMessagePanel.h"
#include "ui_ChatMessagePanel.h"
#include <QDebug>

ChatMessagePanel::ChatMessagePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatMessagePanel)
{
    ui->setupUi(this);
}


ChatMessagePanel::ChatMessagePanel(QWidget *parent, QString userName, QString msg, QColor backgroundColor)
    :QWidget(parent),
      ui(new Ui::ChatMessagePanel)
{
    ui->setupUi(this);
    ui->labelUserName->setText(userName);

    msg = msg.replace(QRegExp("<.+?>"), "");//scape html tags
    msg = msg.replace("\n", "<br/>");
    msg = replaceLinksInString(msg);
    ui->labelMessage->setText(msg);

    QColor nameColor = backgroundColor;
    nameColor.setHsvF(backgroundColor.hueF(), backgroundColor.saturationF(), backgroundColor.valueF() - 0.3);
    ui->labelUserName->setStyleSheet("background-color: " + colorToCSS(nameColor));
    ui->labelMessage->setStyleSheet("background-color: " + colorToCSS(backgroundColor));
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
