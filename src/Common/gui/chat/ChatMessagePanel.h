#ifndef CHATMESSAGEPANEL_H
#define CHATMESSAGEPANEL_H

#include <QFrame>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtGui/QPainterPath>

namespace Ui {
class ChatMessagePanel;
}

class Emojifier;

class ChatMessagePanel : public QFrame
{
    Q_OBJECT

public:
    ChatMessagePanel(QWidget *parent, const QString &userFullName, const QString &msg, const QColor &backgroundColor,
                     const QColor &textColor, bool showTranslationButton, bool showBlockButton, Emojifier *emojifier = nullptr);
    ~ChatMessagePanel();
    void setPrefferedTranslationLanguage(const QString &targetLanguage);
    void translate();
    QString getUserFullName() const;

    void setFontSizeOffset(qint8 sizeOffset);

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
    void blockingUser(const QString &userFullName);

protected:
    void changeEvent(QEvent *) override;
    void focusInEvent(QFocusEvent *) override;
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    bool event(QEvent *e) override;

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

    struct FontDetails
    {
        QString unit = "pt"; // pt or px (point or pixel)
        qreal size = -1;
    };

    FontDetails originalMessageFont; // stored after the widget is polished
    FontDetails originalAuthorFont;

    const static quint32 ARROW_WIDTH = 10;

    void buildPainterPath();

    QString userFullName;

    Emojifier *emojifier;

    static QString replaceLinksInString(const QString &string);

    void initialize(const QString &userFullName, const QString &msg, bool showTranslationButton, bool showBlockButton);

    void setTranslatedMessage(const QString &translatedMessage);

    void setMessageLabelText(const QString &msg);

    void adjustContentMargins();

    void downloadImage(const QString &link);
    void insertImage(const QImage &image, const QString &link);

    static FontDetails getFontDetails(const QFont &f);

    static QString buildFontStyleSheet(const FontDetails &fontDetails, qint8 fontSizeOffset);
};

inline QString ChatMessagePanel::getUserFullName() const
{
    return userFullName;
}

#endif // CHATMESSAGEPANEL_H
