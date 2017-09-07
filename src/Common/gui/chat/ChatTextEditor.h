#ifndef CHATTEXTEDIT_H
#define CHATTEXTEDIT_H

#include <QTextBrowser>

class ChatTextEditor : public QTextBrowser
{
public:
    ChatTextEditor(QWidget *parent);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    int getMinimumHeight() const;

};

#endif // CHATTEXTEDIT_H
