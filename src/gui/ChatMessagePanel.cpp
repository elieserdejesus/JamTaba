#include "ChatMessagePanel.h"
#include "ui_ChatMessagePanel.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMetaMethod>
#include "../src/persistence/Settings.h"// for chat translation


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

void ChatMessagePanel::initialize(QString userName, QString msg, QColor userNameBackgroundColor, QColor msgBackgroundColor, QColor textColor, bool drawBorder ){
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
    ui->translateButton->setStyleSheet(buildCssString(userNameBackgroundColor, textColor, drawBorder));

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
            QString encodedText(QUrl::toPercentEncoding(originalText));
            QString url = "http://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl="+ preferredTargetTranslationLanguage +"&dt=t&q=" + encodedText;
            QNetworkRequest req;
            req.setUrl(QUrl(url));
            req.setOriginatingObject(this);
            req.setRawHeader( "User-Agent" , "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.71 Safari/537.36" );
            qDebug() << url;
            QNetworkReply* reply = httpClient->get(req);
            QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(on_networkReplyError(QNetworkReply::NetworkError)));

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

void ChatMessagePanel::setPrefferedTranslationLanguage(QString targetLanguage){
    this->preferredTargetTranslationLanguage = targetLanguage;
}

void ChatMessagePanel::on_networkReplyError(QNetworkReply::NetworkError ){
    QString errorString = ((QNetworkReply*) QObject::sender())->errorString();
    qCritical() << "Translation error:" << errorString;
}

void ChatMessagePanel::on_networkReplyFinished(QNetworkReply *reply){
    QString data(reply->readAll());
    ChatMessagePanel* messagePanel = static_cast<ChatMessagePanel*>( reply->request().originatingObject());
    int startSlash = data.indexOf(QRegExp("\""));
    int endSlash = data.indexOf(QRegExp("\""), startSlash + 1 ) ;
    QString translated = data.mid(startSlash+1, endSlash - startSlash - 1);
    if(translated.isEmpty()){
        translated = "translation error!";
    }
    messagePanel->setTranslatedMessage(translated);

    reply->manager()->deleteLater();
    reply->deleteLater();
}

void ChatMessagePanel::setTranslatedMessage(QString translatedMessage){
    this->translatedText = translatedMessage;
    this->ui->labelMessage->setText(translatedMessage);
}
