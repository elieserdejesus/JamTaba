#ifndef EMOJIWIDGET_H
#define EMOJIWIDGET_H

#include <QFrame>
#include <QBoxLayout>
#include <QScrollArea>
#include <QCompleter>
#include <QToolButton>

class EmojiManager;


//class EmojiCompleter : public QCompleter
//{

//public:
//    explicit EmojiCompleter(QAbstractItemModel *model, QObject *parent = 0);

//    static const int completeRole;

//    QString pathFromIndex(const QModelIndex &index) const;
//};



class EmojiWidget : public QFrame
{
    Q_OBJECT

public:
    EmojiWidget(EmojiManager *emojiManager, QWidget *parent = nullptr);

public slots:
    void setVisible(bool visible) override;

signals:
    void emojiSelected(const QString &emojiCode);

private slots:
    void showCategory(const QString &category);

private:

    EmojiManager *emojiManager;

    QLayout *createCategoriesLayout();
    void clearLayout(QLayout *layout);

    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;

    QToolButton *recentButton;


    static const uint BUTTONS_SIZE;
};

#endif // EMOJIWIDGET_H
