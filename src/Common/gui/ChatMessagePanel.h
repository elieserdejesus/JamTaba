#ifndef CHATMESSAGEPANEL_H
#define CHATMESSAGEPANEL_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class ChatMessagePanel;
}

class ChatMessagePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChatMessagePanel(QWidget *parent);
    ChatMessagePanel(QWidget *parent, const QString &userName, const QString &msg, const QColor &userNameBackgroundColor,
                     const QColor &msgBackgroundColor, const QColor &textColor, bool showTranslationButton);
    ~ChatMessagePanel();
    void setPrefferedTranslationLanguage(const QString &targetLanguage);
    void translate();

signals:
    void startingTranslation();
    void translationFinished();


protected:
    void changeEvent(QEvent *) override;

private slots:
    void on_translateButton_clicked();
    void on_networkReplyFinished(QNetworkReply *);
    void on_networkReplyError(QNetworkReply::NetworkError);

private:
    QString originalText;
    QString translatedText;
    Ui::ChatMessagePanel *ui;
    QString preferredTargetTranslationLanguage;
    static QString replaceLinksInString(const QString &string);

    static QString buildCssString(const QColor &bgColor, const QColor &textColor);

    void initialize(const QString &userName, const QString &msg, const QColor &userNameBackgroundColor,
                    const QColor &msgBackgroundColor, const QColor &textColor, bool showTranslationButton);

    void setTranslatedMessage(const QString &translatedMessage);
};

#endif // CHATMESSAGEPANEL_H
