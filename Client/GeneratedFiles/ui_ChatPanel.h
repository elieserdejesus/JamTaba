/********************************************************************************
** Form generated from reading UI file 'ChatPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATPANEL_H
#define UI_CHATPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatPanel
{
public:
    QGridLayout *gridLayout;
    QLineEdit *chatText;
    QLabel *iconLabel;
    QPushButton *buttonClear;
    QScrollArea *chatScroll;
    QWidget *scrollContent;

    void setupUi(QWidget *ChatPanel)
    {
        if (ChatPanel->objectName().isEmpty())
            ChatPanel->setObjectName(QStringLiteral("ChatPanel"));
        ChatPanel->resize(320, 412);
        gridLayout = new QGridLayout(ChatPanel);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(6, 6, 6, 6);
        chatText = new QLineEdit(ChatPanel);
        chatText->setObjectName(QStringLiteral("chatText"));

        gridLayout->addWidget(chatText, 2, 1, 1, 1);

        iconLabel = new QLabel(ChatPanel);
        iconLabel->setObjectName(QStringLiteral("iconLabel"));
        iconLabel->setTextFormat(Qt::RichText);
        iconLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/chat.png")));

        gridLayout->addWidget(iconLabel, 2, 0, 1, 1);

        buttonClear = new QPushButton(ChatPanel);
        buttonClear->setObjectName(QStringLiteral("buttonClear"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonClear->sizePolicy().hasHeightForWidth());
        buttonClear->setSizePolicy(sizePolicy);
        buttonClear->setFlat(true);

        gridLayout->addWidget(buttonClear, 2, 2, 1, 1, Qt::AlignHCenter|Qt::AlignVCenter);

        chatScroll = new QScrollArea(ChatPanel);
        chatScroll->setObjectName(QStringLiteral("chatScroll"));
        chatScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        chatScroll->setWidgetResizable(true);
        scrollContent = new QWidget();
        scrollContent->setObjectName(QStringLiteral("scrollContent"));
        scrollContent->setGeometry(QRect(0, 0, 306, 369));
        scrollContent->setMinimumSize(QSize(0, 100));
        chatScroll->setWidget(scrollContent);

        gridLayout->addWidget(chatScroll, 0, 0, 1, 3);


        retranslateUi(ChatPanel);

        QMetaObject::connectSlotsByName(ChatPanel);
    } // setupUi

    void retranslateUi(QWidget *ChatPanel)
    {
        ChatPanel->setWindowTitle(QApplication::translate("ChatPanel", "Form", 0));
        iconLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        buttonClear->setToolTip(QApplication::translate("ChatPanel", "clear the messages ...", 0));
#endif // QT_NO_TOOLTIP
        buttonClear->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ChatPanel: public Ui_ChatPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATPANEL_H
