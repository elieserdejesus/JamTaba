#include "ChatMessagePanel.h"
#include "ui_ChatMessagePanel.h"
#include "log/Logging.h"
#include "EmojiManager.h"

#include <QTime>
#include <QPainter>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

ChatMessagePanel::ChatMessagePanel(QWidget *parent, const QString &userName, const QString &msg,
                                   const QColor &backgroundColor, const QColor &textColor,
                                   bool showTranslationButton, bool showBlockButton, Emojifier *emojifier) :
    QFrame(parent),
    ui(new Ui::ChatMessagePanel),
    userName(userName),
    emojifier(emojifier),
    backgroundColor(backgroundColor),
    showArrow(true),
    arrowSide(ChatMessagePanel::LeftSide)
{
    ui->setupUi(this);

    if (emojifier)
        emojifiedText = emojifier->emojify(msg);
    else
        emojifiedText = msg;

    initialize(userName, msg, showTranslationButton, showBlockButton);

    connect(ui->blockButton, &QPushButton::clicked, this, &ChatMessagePanel::fireBlockingUserSignal);

    setBackgroundRole(QPalette::NoRole);

    setStyleSheet(QString("color: %1;").arg(textColor.name()));

    buildPainterPath();

    adjustContentMargins();
}

bool ChatMessagePanel::event(QEvent *e)
{
    if (e->type() == QEvent::Polish) {
        originalMessageFont = ChatMessagePanel::getFontDetails(ui->labelMessage->font());
        originalAuthorFont = ChatMessagePanel::getFontDetails(ui->labelUserName->font());
    }

    return QFrame::event(e);
}

ChatMessagePanel::FontDetails ChatMessagePanel::getFontDetails(const QFont &f)
{
    ChatMessagePanel::FontDetails fontDetails;

    fontDetails.unit = "px";
    fontDetails.size = f.pixelSize();

    if (f.pixelSize() <= 0) {
        fontDetails.unit = "pt";
        fontDetails.size = f.pointSizeF();
    }

    return fontDetails;
}

QString ChatMessagePanel::buildFontStyleSheet(const FontDetails &fontDetails, qint8 fontSizeOffset)
{
    return QString("font-size: %1%2;")
                .arg(fontDetails.size + fontSizeOffset)
                .arg(fontDetails.unit);
}

void ChatMessagePanel::setFontSizeOffset(qint8 sizeOffset)
{
    ensurePolished(); // compute original fonts size and unit (px or pt)

    ui->labelMessage->setStyleSheet(buildFontStyleSheet(originalMessageFont, sizeOffset));
    ui->labelUserName->setStyleSheet(buildFontStyleSheet(originalAuthorFont, sizeOffset));
}

void ChatMessagePanel::adjustContentMargins()
{
    int left = (arrowSide == ChatMessagePanel::LeftSide ? ARROW_WIDTH : 0) + 3;
    int top = 3;
    int right = (arrowSide == ChatMessagePanel::RightSide ? ARROW_WIDTH : 0) + 3;
    int bottom = 3;
    QMargins margins(left, top, right, bottom);

    ui->verticalLayout->setContentsMargins(margins);
}

void ChatMessagePanel::setShowArrow(bool showArrow)
{
    this->showArrow = showArrow;
    if (!showArrow)
        arrowSide = ChatMessagePanel::LeftSide;

    buildPainterPath();

    adjustContentMargins();

    update();
}

void ChatMessagePanel::setArrowSide(ArrowSide side)
{
    arrowSide = side;

    buildPainterPath();

    adjustContentMargins();

    update();
}

void ChatMessagePanel::buildPainterPath()
{
    painterPath = QPainterPath();
    shadowPath = QPainterPath();

    const qreal round = showArrow ? 10 : 3;

    const qreal arrowHeight = showArrow ? ARROW_WIDTH * 0.8 : 0;

    qreal w = width();
    qreal h = height();

    qreal left = 0.0;
    qreal right = w - 1.0;
    qreal bottom = h - 1.0;
    qreal top = 0.0;

    QList<QPainterPath *> paths;
    paths.append(&painterPath);
    paths.append(&shadowPath);

    if (arrowSide == ChatMessagePanel::LeftSide) {
        painterPath.moveTo(left, top);

        painterPath.lineTo(right - round, top); // top line
        painterPath.quadTo(right, top, right, top + round); // top right corner
        painterPath.lineTo(right, bottom - round); // right line
        painterPath.quadTo(right, bottom, right - round, bottom); // bottom right corner

        shadowPath.moveTo(right - round, bottom);

        for (auto path : paths) {
            path->lineTo(left + round + ARROW_WIDTH, bottom); // bottom line
            path->quadTo(left + ARROW_WIDTH, bottom, left + ARROW_WIDTH, bottom - round); // bottom left corner
            path->lineTo(left + ARROW_WIDTH, top + arrowHeight);
            path->lineTo(left, top);
        }
    }
    else {
        painterPath.moveTo(right, top);
        painterPath.lineTo(left + round, top); // top line
        painterPath.quadTo(left, top, left, top + round); // top left corner
        painterPath.lineTo(left, bottom - round); // left line
        painterPath.quadTo(left, bottom, left + round, bottom); // bottom left corner

        shadowPath.moveTo(left + round, bottom);

        for (auto path : paths) {
            path->lineTo(right - round - ARROW_WIDTH, bottom); // bottom line
            path->quadTo(right - ARROW_WIDTH, bottom, right - ARROW_WIDTH, bottom - round); // bottom right corner
            path->lineTo(right - ARROW_WIDTH, top + arrowHeight);
            path->lineTo(right, top);
        }
    }
}

void ChatMessagePanel::resizeEvent(QResizeEvent *ev)
{
    QFrame::resizeEvent(ev);

    buildPainterPath();

    ui->labelMessage->setMaximumWidth(parentWidget()->width() - 30);
}

void ChatMessagePanel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    static const QColor shadowColor(0, 0, 0, 90);

    if (showArrow) {
        painter.setPen(shadowColor);
        painter.drawPath(shadowPath);
    }

    painter.setPen(Qt::NoPen);
    painter.fillPath(painterPath, backgroundColor);
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

void ChatMessagePanel::initialize(const QString &userName, const QString &msg, bool showTranslationButton, bool showBlockButton)
{
    if (!userName.isEmpty() && !userName.isNull()) {
        ui->labelUserName->setText(userName);
    }
    else {
        ui->labelUserName->setVisible(false);
    }

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

    //ui->labelMessage->setText("..."); // translating
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
