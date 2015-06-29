#ifndef CHATMESSAGEPANEL_H
#define CHATMESSAGEPANEL_H

#include <QWidget>

namespace Ui {
class ChatMessagePanel;
}

class ChatMessagePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChatMessagePanel(QWidget *parent);
    ChatMessagePanel(QWidget *parent, QString userName, QString msg,
                     QColor backgroundColor);
    ~ChatMessagePanel();

private:
    Ui::ChatMessagePanel *ui;
    static QString colorToCSS(QColor);
    static QString replaceLinksInString(QString str);
};

#endif // CHATMESSAGEPANEL_H
