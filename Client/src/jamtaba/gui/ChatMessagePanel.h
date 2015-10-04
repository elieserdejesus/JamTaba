#ifndef CHATMESSAGEPANEL_H
#define CHATMESSAGEPANEL_H

#include <QWidget>
#include <QNetworkAccessManager>

class QNetworkReply;

namespace Ui {
class ChatMessagePanel;
}

class ChatMessagePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChatMessagePanel(QWidget *parent);
    //ChatMessagePanel(QWidget *parent, QString userName, QString msg, QColor backgroundColor);
    ChatMessagePanel(QWidget *parent, QString userName, QString msg, QColor userNameBackgroundColor, QColor msgBackgroundColor, QColor textColor, bool drawBorder=true);
    ~ChatMessagePanel();

//    QSize sizeHint() const;
//    QSize minimumSizeHint() const;

private slots:
    void on_translateButton_clicked();
    void on_networkReplyFinished(QNetworkReply*);

private:
    QString originalText;
    QString translatedText;
    Ui::ChatMessagePanel *ui;
    static QString colorToCSS(QColor);
    static QString replaceLinksInString(QString str);

    static QString buildCssString(QColor bgColor, QColor textColor, bool drawBorder);

    void initialize(QString userName, QString msg, QColor userNameBackgroundColor, QColor msgBackgroundColor, QColor textColor, bool drawBorder);

    void setTranslatedMessage(QString translatedMessage);
};

#endif // CHATMESSAGEPANEL_H
