#ifndef CHATMESSAGEPANEL_H
#define CHATMESSAGEPANEL_H

#include <QFrame>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class ChatMessagePanel;
}

class Emojifier;

class ChatMessagePanel : public QFrame
{
    Q_OBJECT

public:
    ChatMessagePanel(QWidget *parent, const QString &userName, const QString &msg, const QColor &backgroundColor,
                     const QColor &textColor, bool showTranslationButton, bool showBlockButton, Emojifier *emojifier = nullptr);
    ~ChatMessagePanel();
    void setPrefferedTranslationLanguage(const QString &targetLanguage);
    void translate();
    QString getUserName() const;

    enum ArrowSide
    {
        LeftSide,
        RightSide
    };

    void setShowArrow(bool showArrow);
    void setArrowSide(ArrowSide side);

signals:
    void startingTranslation();
    void translationFinished();
    void blockingUser(const QString &userName);

protected:
    void changeEvent(QEvent *) override;
    void focusInEvent(QFocusEvent *) override;
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;

private slots:
    void on_translateButton_clicked();
    void fireBlockingUserSignal();
    void handleAvailableTranslation(QNetworkReply *);
    void handleTranslationError(QNetworkReply::NetworkError);

private:
    QString originalText;
    QString translatedText;
    QString emojifiedText;
    Ui::ChatMessagePanel *ui;
    QString preferredTargetTranslationLanguage;

    QPainterPath painterPath;
    QPainterPath shadowPath;
    bool showArrow;
    ArrowSide arrowSide;

    QColor backgroundColor;

    const static quint32 ARROW_WIDTH = 10;

    void buildPainterPath();

    QString userName;

    Emojifier *emojifier;

    static QString replaceLinksInString(const QString &string);

    void initialize(const QString &userName, const QString &msg, bool showTranslationButton, bool showBlockButton);

    void setTranslatedMessage(const QString &translatedMessage);

    void setMessageLabelText(const QString &msg);

    void adjustContentMargins();

};

inline QString ChatMessagePanel::getUserName() const
{
    return userName;
}

#endif // CHATMESSAGEPANEL_H
