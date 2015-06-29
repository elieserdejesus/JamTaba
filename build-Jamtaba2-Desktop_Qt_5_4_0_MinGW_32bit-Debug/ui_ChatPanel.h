/********************************************************************************
** Form generated from reading UI file 'ChatPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
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
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatPanel
{
public:
    QGridLayout *gridLayout;
    QLabel *iconLabel;
    QLineEdit *chatText;
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
        iconLabel = new QLabel(ChatPanel);
        iconLabel->setObjectName(QStringLiteral("iconLabel"));
        iconLabel->setTextFormat(Qt::RichText);
        iconLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/chat.png")));

        gridLayout->addWidget(iconLabel, 1, 0, 1, 1);

        chatText = new QLineEdit(ChatPanel);
        chatText->setObjectName(QStringLiteral("chatText"));

        gridLayout->addWidget(chatText, 1, 1, 1, 1);

        chatScroll = new QScrollArea(ChatPanel);
        chatScroll->setObjectName(QStringLiteral("chatScroll"));
        chatScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        chatScroll->setWidgetResizable(true);
        scrollContent = new QWidget();
        scrollContent->setObjectName(QStringLiteral("scrollContent"));
        scrollContent->setGeometry(QRect(0, 0, 306, 372));
        scrollContent->setMinimumSize(QSize(0, 100));
        chatScroll->setWidget(scrollContent);

        gridLayout->addWidget(chatScroll, 0, 0, 1, 2);


        retranslateUi(ChatPanel);

        QMetaObject::connectSlotsByName(ChatPanel);
    } // setupUi

    void retranslateUi(QWidget *ChatPanel)
    {
        ChatPanel->setWindowTitle(QApplication::translate("ChatPanel", "Form", 0));
        iconLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ChatPanel: public Ui_ChatPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATPANEL_H
