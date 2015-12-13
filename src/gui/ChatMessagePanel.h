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
    ChatMessagePanel(QWidget *parent, QString userName, QString msg, QColor userNameBackgroundColor, QColor msgBackgroundColor, QColor textColor, bool showTranslationButton);
    ~ChatMessagePanel();
    void setPrefferedTranslationLanguage(QString targetLanguage);
    void translate();
private slots:
    void on_translateButton_clicked();
    void on_networkReplyFinished(QNetworkReply*);
    void on_networkReplyError(QNetworkReply::NetworkError);

private:
    QString originalText;
    QString translatedText;
    Ui::ChatMessagePanel *ui;
    QString preferredTargetTranslationLanguage;
    static QString colorToCSS(QColor);
    static QString replaceLinksInString(QString str);

    static QString buildCssString(QColor bgColor, QColor textColor);

    void initialize(QString userName, QString msg, QColor userNameBackgroundColor, QColor msgBackgroundColor, QColor textColor, bool showTranslationButton);

    void setTranslatedMessage(QString translatedMessage);
};

#endif // CHATMESSAGEPANEL_H
