#include "ChatMessagePanel.h"
#include "ui_ChatMessagePanel.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMetaMethod>
#include "log/Logging.h"

ChatMessagePanel::ChatMessagePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatMessagePanel)
{
    ui->setupUi(this);
}

ChatMessagePanel::ChatMessagePanel(QWidget *parent, const QString &userName, const QString &msg,
                                   const QColor &userNameBackgroundColor,
                                   const QColor &msgBackgroundColor, const QColor &textColor,
                                   bool showTranslationButton) :
    QWidget(parent),
    ui(new Ui::ChatMessagePanel)
{
    ui->setupUi(this);
    initialize(userName, msg, userNameBackgroundColor, msgBackgroundColor, textColor,
               showTranslationButton);
}

void ChatMessagePanel::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);

    }
    QWidget::changeEvent(e);
}

void ChatMessagePanel::initialize(const QString &userName, const QString &msg,
                                  const QColor &userNameBackgroundColor,
                                  const QColor &msgBackgroundColor, const QColor &textColor,
                                  bool showTranslationButton)
{
    if (!userName.isEmpty() && !userName.isNull()) {
        ui->labelUserName->setText(userName + ":");
        ui->labelUserName->setStyleSheet(buildCssString(userNameBackgroundColor, textColor));
    } else {
        ui->labelUserName->setVisible(false);
    }

    QString newMessage(msg);
    newMessage = newMessage.replace(QRegExp("<.+?>"), "");// scape html tags
    newMessage = newMessage.replace("\n", "<br/>");
    newMessage = replaceLinksInString(newMessage);
    ui->labelMessage->setText(newMessage);
    ui->labelMessage->setStyleSheet(buildCssString(msgBackgroundColor, textColor));

    if (showTranslationButton)
        ui->translateButton->setStyleSheet(buildCssString(userNameBackgroundColor, textColor));
    else
        ui->translateButton->setVisible(false);

    this->originalText = msg;
}

QString ChatMessagePanel::buildCssString(const QColor &bgColor, const QColor &textColor)
{
    QString css = "background-color: " + bgColor.name() + ";";
    css += "color: " + textColor.name() + ";";
    return css;
}

QString ChatMessagePanel::replaceLinksInString(const QString &string)
{
    QString regex = "((?:https?|ftp|www)://\\S+)";
    return QString(string).replace(QRegExp(regex), "<a href=\"\\1\">\\1</a>");
}

ChatMessagePanel::~ChatMessagePanel()
{
    delete ui;
}

void ChatMessagePanel::translate()
{
    emit startingTranslation();

    QNetworkAccessManager *httpClient = new QNetworkAccessManager(this);
    QString encodedText(QUrl::toPercentEncoding(originalText));
    QString url = "http://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl="
                  + preferredTargetTranslationLanguage +"&dt=t&q=" + encodedText;
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    req.setOriginatingObject(this);
    req.setRawHeader("User-Agent",
                     "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.71 Safari/537.36");

    qCDebug(jtGUI) << "Translating:" << url;

    QNetworkReply *reply = httpClient->get(req);

    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
                     SLOT(on_networkReplyError(QNetworkReply::NetworkError)));

    QObject::connect(httpClient, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(on_networkReplyFinished(QNetworkReply *)));

    ui->labelMessage->setText("...");// translating ...
}

void ChatMessagePanel::on_translateButton_clicked()
{
    if (ui->translateButton->isChecked()) {
        if (translatedText.isEmpty())
            translate();
        else
            ui->labelMessage->setText(translatedText);
    } else {
        ui->labelMessage->setText(originalText);
    }
}

void ChatMessagePanel::setPrefferedTranslationLanguage(const QString &targetLanguage)
{
    this->preferredTargetTranslationLanguage = targetLanguage;
}

void ChatMessagePanel::on_networkReplyError(QNetworkReply::NetworkError)
{
    QString errorString = (qobject_cast<QNetworkReply *>(QObject::sender()))->errorString();
    qCritical() << "Translation error:" << errorString;

    emit translationFinished();

    ui->labelMessage->setText(originalText);// restore the original text
}

void ChatMessagePanel::on_networkReplyFinished(QNetworkReply *reply)
{
    QString data(reply->readAll());
    ChatMessagePanel *messagePanel
        = qobject_cast<ChatMessagePanel *>(reply->request().originatingObject());
    int startSlash = data.indexOf(QRegExp("\""));
    int endSlash = data.indexOf(QRegExp("\""), startSlash + 1);
    QString translatedText = data.mid(startSlash+1, endSlash - startSlash - 1);
    if (translatedText.isEmpty())
        translatedText = "translation error!";
    messagePanel->setTranslatedMessage(translatedText);

    reply->manager()->deleteLater();
    reply->deleteLater();

    emit translationFinished();
}

void ChatMessagePanel::setTranslatedMessage(const QString &translatedMessage)
{
    ui->translateButton->setChecked(true);
    this->translatedText = translatedMessage;
    this->ui->labelMessage->setText(translatedMessage);
}
