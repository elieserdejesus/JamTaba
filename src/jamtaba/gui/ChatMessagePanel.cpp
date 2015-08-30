#include "ChatMessagePanel.h"
#include "ui_ChatMessagePanel.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMetaMethod>


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
    //QObject::connect( translationHttpClient, SIGNAL()
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

    this->originalText = msg;
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

void ChatMessagePanel::on_translateButton_clicked(){
    if(ui->translateButton->isChecked()){
        if(translatedText.isEmpty()){
            QNetworkAccessManager* httpClient = new QNetworkAccessManager(this);
            QLocale userLocale;
            QString targetLanguage =  userLocale.bcp47Name().left(2);
            QString encodedText(QUrl::toPercentEncoding(originalText));
            QString url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl="+ targetLanguage +"&dt=t&q=" + encodedText;
            QNetworkRequest req;//(QUrl(url));
            req.setUrl(QUrl(url));
            req.setOriginatingObject(this);
            httpClient->get(req);

            QObject::connect(httpClient, SIGNAL(finished(QNetworkReply*)), this, SLOT(on_networkReplyFinished(QNetworkReply*)));
        }
        else{
            ui->labelMessage->setText(translatedText);
        }
    }
    else{
        ui->labelMessage->setText(originalText);
    }
}

void ChatMessagePanel::on_networkReplyFinished(QNetworkReply *reply){
    QString data(reply->readAll());
    ChatMessagePanel* messagePanel = static_cast<ChatMessagePanel*>( reply->request().originatingObject());
    //qWarning() << data;

    int startSlash = data.indexOf(QRegExp("\""));
    int endSlash = data.indexOf(QRegExp("\""), startSlash + 1 ) ;
    //qWarning() << "start: " << startSlash;
    //qWarning() << "end:" <<endSlash;

    messagePanel->setTranslatedMessage(data.mid(startSlash+1, endSlash - startSlash - 1));

    reply->manager()->deleteLater();
    reply->deleteLater();
}

void ChatMessagePanel::setTranslatedMessage(QString translatedMessage){
    this->translatedText = translatedMessage;
    this->ui->labelMessage->setText(translatedMessage);
}
