#ifndef CHATMESSAGEPANEL_H
#define CHATMESSAGEPANEL_H

#include <QFrame>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class ChatMessagePanel;
}

class ChatMessagePanel : public QFrame
{
    Q_OBJECT

public:
    explicit ChatMessagePanel(QWidget *parent);
    ChatMessagePanel(QWidget *parent, const QString &userName, const QString &msg, const QColor &userNameBackgroundColor, const QColor &textColor, bool showTranslationButton, bool showBlockButton);
    ~ChatMessagePanel();
    void setPrefferedTranslationLanguage(const QString &targetLanguage);
    void translate();
    QString getUserName() const;

signals:
    void startingTranslation();
    void translationFinished();
    void blockingUser(const QString &userName);

protected:
    void changeEvent(QEvent *) override;
    void focusInEvent(QFocusEvent *) override;

private slots:
    void on_translateButton_clicked();
    void fireBlockingUserSignal();
    void on_networkReplyFinished(QNetworkReply *);
    void on_networkReplyError(QNetworkReply::NetworkError);

private:
    QString originalText;
    QString translatedText;
    Ui::ChatMessagePanel *ui;
    QString preferredTargetTranslationLanguage;

    QString userName;

    static QString replaceLinksInString(const QString &string);

    static QString buildCssString(const QColor &bgColor, const QColor &textColor);

    void initialize(const QString &userName, const QString &msg,
                    const QColor &msgBackgroundColor, const QColor &textColor, bool showTranslationButton, bool showBlockButton);

    void setTranslatedMessage(const QString &translatedMessage);

    void setMessageLabelText(const QString &msg);

};

inline QString ChatMessagePanel::getUserName() const
{
    return userName;
}

#endif // CHATMESSAGEPANEL_H
