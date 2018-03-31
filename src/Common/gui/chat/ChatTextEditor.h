#ifndef CHATTEXTEDIT_H
#define CHATTEXTEDIT_H

#include <QTextBrowser>

class ChatTextEditor : public QTextBrowser
{
public:
    explicit ChatTextEditor(QWidget *parent);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private slots:
    void updateWidth();

private:
    quint32 computeBestWidth() const;
    quint32 bestWidth;

    uint getEmojisCount() const;

};

#endif // CHATTEXTEDIT_H
