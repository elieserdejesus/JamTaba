#include "ChatMessagePanel.h"
#include "ui_ChatMessagePanel.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMetaMethod>
#include "log/Logging.h"
#include <QTime>
#include "EmojiManager.h"

ChatMessagePanel::ChatMessagePanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ChatMessagePanel)
{
    ui->setupUi(this);
}

ChatMessagePanel::ChatMessagePanel(QWidget *parent, const QString &userName, const QString &msg,
                                   const QColor &userNameBackgroundColor,
                                   const QColor &textColor,
                                   bool showTranslationButton, bool showBlockButton, Emojifier *emojifier) :
    QFrame(parent),
    ui(new Ui::ChatMessagePanel),
    userName(userName),
    emojifier(emojifier)
{
    ui->setupUi(this);

    if (emojifier)
        emojifiedText = emojifier->emojify(msg);
    else
        emojifiedText = msg;

    initialize(userName, msg, userNameBackgroundColor, textColor, showTranslationButton, showBlockButton);

    connect(ui->blockButton, &QPushButton::clicked, this, &ChatMessagePanel::fireBlockingUserSignal);
}

void ChatMessagePanel::focusInEvent(QFocusEvent *ev)
{
    QFrame::focusInEvent(ev);
    ui->labelMessage->setFocus();
}

void ChatMessagePanel::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }

    QWidget::changeEvent(e);
}

void ChatMessagePanel::initialize(const QString &userName, const QString &msg,
                                  const QColor &msgBackgroundColor, const QColor &textColor,
                                  bool showTranslationButton, bool showBlockButton)
{
    if (!userName.isEmpty() && !userName.isNull()) {
        ui->labelUserName->setText(userName);
    }
    else {
        ui->labelUserName->setVisible(false);
    }

    setStyleSheet(buildCssString(msgBackgroundColor, textColor));

    setMessageLabelText(emojifiedText);

    ui->translateButton->setVisible(showTranslationButton);

    ui->blockButton->setVisible(showBlockButton);

    originalText = msg;
}

void ChatMessagePanel::setMessageLabelText(const QString &msg)
{
    QString newMessage(msg);
    newMessage = newMessage.replace(QRegExp("<.+?>"), ""); // scape html tags
    newMessage = newMessage.replace("\n", "<br/>");
    newMessage = replaceLinksInString(newMessage);

    ui->labelMessage->setText(newMessage);
}

QString ChatMessagePanel::buildCssString(const QColor &bgColor, const QColor &textColor)
{
    QString css = "ChatMessagePanel { ";
    css += " background-color: " + bgColor.name(QColor::HexArgb) + ";";
    css += " color: " + textColor.name() + ";";
    css += "}";
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

    auto httpClient = new QNetworkAccessManager(this);
    QString encodedText(QUrl::toPercentEncoding(originalText));
    QString url = QString("http://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl=%1&dt=t&q=%2")
            .arg(preferredTargetTranslationLanguage)
            .arg(encodedText);

    QNetworkRequest req;
    req.setUrl(QUrl(url));
    req.setOriginatingObject(this);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.71 Safari/537.36");

    qCDebug(jtGUI) << "Translating:" << url;

    QNetworkReply *reply = httpClient->get(req);

    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleTranslationError(QNetworkReply::NetworkError)));

    QObject::connect(httpClient, SIGNAL(finished(QNetworkReply *)), this, SLOT(handleAvailableTranslation(QNetworkReply *)));

    ui->labelMessage->setText("..."); // translating
}

void ChatMessagePanel::on_translateButton_clicked()
{
    if (ui->translateButton->isChecked()) {
        if (translatedText.isEmpty()) {
            translate();
        }
        else {
            setMessageLabelText("<i>" + translatedText + "</i>");
        }
    }
    else {
        setMessageLabelText(emojifiedText);
    }
}

void ChatMessagePanel::fireBlockingUserSignal()
{
    emit blockingUser(userName);
}

void ChatMessagePanel::setPrefferedTranslationLanguage(const QString &targetLanguage)
{
    preferredTargetTranslationLanguage = targetLanguage;
}

void ChatMessagePanel::handleTranslationError(QNetworkReply::NetworkError)
{
    auto sender = qobject_cast<QNetworkReply *>(QObject::sender());
    if (!sender)
        return;

    qCritical() << "Translation error:" << sender->errorString();

    setTranslatedMessage(tr("Translation error!"));

    emit translationFinished();
}

void ChatMessagePanel::handleAvailableTranslation(QNetworkReply *reply)
{
    auto messagePanel = qobject_cast<ChatMessagePanel *>(reply->request().originatingObject());
    if (!messagePanel)
        return;

    if (reply->error() == QNetworkReply::NoError) {

        QString downloadedData(reply->readAll());

        int startSlash = downloadedData.indexOf(QRegExp("\""));
        int endSlash = downloadedData.indexOf(QRegExp("\""), startSlash + 1);

        QString translatedText = downloadedData.mid(startSlash+1, endSlash - startSlash - 1);
        if (translatedText.isEmpty())
            translatedText = "translation error!";

        messagePanel->setTranslatedMessage(translatedText);
    }

    if (reply->manager())
        reply->manager()->deleteLater();

    reply->deleteLater();

    emit translationFinished();
}

void ChatMessagePanel::setTranslatedMessage(const QString &translatedMessage)
{
    ui->translateButton->setChecked(true);
    translatedText = emojifier ? emojifier->emojify(translatedMessage) : translatedMessage;
    setMessageLabelText("<i>" + translatedText + "</i>");
}
