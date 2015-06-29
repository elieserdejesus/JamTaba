#ifndef CHATMESSAGETEXTBROWSER_H
#define CHATMESSAGETEXTBROWSER_H

#include <QTextBrowser>

class ChatMessageTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit ChatMessageTextBrowser(QWidget *parent = 0);
    ~ChatMessageTextBrowser();

signals:

public slots:
};

#endif // CHATMESSAGETEXTBROWSER_H
