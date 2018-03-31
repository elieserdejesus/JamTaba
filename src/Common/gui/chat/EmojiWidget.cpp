#include "EmojiWidget.h"

#include <QGridLayout>
#include <QToolButton>
#include <QIcon>
#include <QDebug>
#include <QScrollBar>
#include <QDateTime>
#include <QAbstractItemModel>

#include "EmojiManager.h"

const uint EmojiWidget::BUTTONS_SIZE = 32;

//const int EmojiCompleter::completeRole = Qt::UserRole + 1;

//EmojiCompleter::EmojiCompleter(QAbstractItemModel *model, QObject *parent) :
//    QCompleter(model, parent)
//{
//    setCaseSensitivity(Qt::CaseInsensitive);
//    setFilterMode(Qt::MatchContains);
//}

//QString EmojiCompleter::pathFromIndex(const QModelIndex &index) const
//{
//    QMap<int, QVariant> data = model()->itemData(index);

//    QString stringCode = data.value(EmojiCompleter::completeRole).toString();

//    uint code = stringCode.toInt(0, 16);

//    return QString::fromUcs4(&code, 1);
//}



EmojiWidget::EmojiWidget(EmojiManager *emojiManager, QWidget *parent) :
    QFrame(parent),
    emojiManager(emojiManager)
{

    Q_ASSERT(emojiManager);

    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto categoriesLayout = createCategoriesLayout();
    mainLayout->addLayout(categoriesLayout);
    mainLayout->setAlignment(categoriesLayout, Qt::AlignTop);

    scrollArea = new QScrollArea();
    scrollArea->setSizeAdjustPolicy(QScrollArea::AdjustToContents);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(new QWidget(this));
    uint preferredHeight = EmojiWidget::BUTTONS_SIZE * 3; // 3 rows
    scrollArea->setMaximumHeight(preferredHeight);
    scrollArea->setMinimumHeight(preferredHeight);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->widget()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    mainLayout->addWidget(scrollArea, 1, Qt::AlignTop);

    setLayout(mainLayout);

    recentButton->setEnabled(emojiManager->hasRecents());

}

void EmojiWidget::setVisible(bool visible)
{
    QFrame::setVisible(visible);

    if (visible) {
        QLayout *gridLayout = scrollArea->widget()->layout();

        if (emojiManager->hasRecents())
            showCategory("Recent");
        else if (!gridLayout)
            showCategory("Smileys & People");
    }
}

void EmojiWidget::showCategory(const QString &category)
{
    if (scrollArea->widget()->layout()) {
        auto gridLayout = qobject_cast<QGridLayout *>(scrollArea->widget()->layout());
        clearLayout(gridLayout);
        delete gridLayout;
    }

    scrollArea->verticalScrollBar()->setValue(0); // scrool to top

    auto gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setHorizontalSpacing(0);
    gridLayout->setVerticalSpacing(0);
    scrollArea->widget()->setLayout(gridLayout);

    int maxCollumns = (width() - scrollArea->verticalScrollBar()->width())/BUTTONS_SIZE;

    QList<Emoji> emojis = emojiManager->getByCategory(category);

    uint index = 0;

    for (const Emoji &emoji : emojis) {
        auto button = new QToolButton();
        button->setIcon(QPixmap(emojiManager->getEmojiIconUrl(emoji)));
        button->setIconSize(QSize(EmojiManager::ICONS_SIZE, EmojiManager::ICONS_SIZE));
        button->setMinimumSize(QSize(EmojiWidget::BUTTONS_SIZE, EmojiWidget::BUTTONS_SIZE));
        QString toolTip = QString(emoji.name).replace(QChar('_'), QString(" "));
        button->setToolTip(toolTip);

        connect(button, &QToolButton::clicked, [=](){
            emojiManager->addRecent(emoji.unifiedCode);

            emit emojiSelected(emoji.unifiedCode);

            recentButton->setEnabled(emojiManager->hasRecents());
        });

        uint row = index / maxCollumns;
        uint collumn = index % maxCollumns;
        gridLayout->addWidget(button, row, collumn, 1, 1, Qt::AlignTop);

        index++;
    }

    scrollArea->widget()->updateGeometry();
    scrollArea->updateGeometry();
}

QLayout *EmojiWidget::createCategoriesLayout()
{
    auto hLayout = new QHBoxLayout();

    for (auto category : emojiManager->getCategories()) {
        auto button = new QToolButton();
        button->setToolTip(category);
        QString imageName = category.split(" ").at(0); // at(0) always return at least the original string
        QString iconPath = QString(":/emoji/categories/%1.png").arg(imageName);
        button->setIcon(QPixmap(iconPath));

        connect(button, &QToolButton::clicked, [=](){
            showCategory(category);
        });

        hLayout->addWidget(button, 1, Qt::AlignCenter);

        if (category == "Recent")
            recentButton = button;
    }

    return hLayout;
}

void EmojiWidget::clearLayout(QLayout *layout)
{
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}
