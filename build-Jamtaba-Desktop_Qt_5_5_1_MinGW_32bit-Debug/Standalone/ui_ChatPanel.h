/********************************************************************************
** Form generated from reading UI file 'ChatPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
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
    QLabel *iconLabel;
    QLineEdit *chatText;
    QPushButton *buttonClear;
    QScrollArea *chatScroll;
    QWidget *scrollContent;

    void setupUi(QWidget *ChatPanel)
    {
        if (ChatPanel->objectName().isEmpty())
            ChatPanel->setObjectName(QStringLiteral("ChatPanel"));
        ChatPanel->resize(249, 231);
        gridLayout = new QGridLayout(ChatPanel);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(6, 6, 6, 6);
        iconLabel = new QLabel(ChatPanel);
        iconLabel->setObjectName(QStringLiteral("iconLabel"));
        iconLabel->setTextFormat(Qt::RichText);
        iconLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/chat.png")));

        gridLayout->addWidget(iconLabel, 3, 0, 1, 1);

        chatText = new QLineEdit(ChatPanel);
        chatText->setObjectName(QStringLiteral("chatText"));

        gridLayout->addWidget(chatText, 3, 1, 1, 1);

        buttonClear = new QPushButton(ChatPanel);
        buttonClear->setObjectName(QStringLiteral("buttonClear"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonClear->sizePolicy().hasHeightForWidth());
        buttonClear->setSizePolicy(sizePolicy);
        buttonClear->setFlat(true);

        gridLayout->addWidget(buttonClear, 3, 2, 1, 1);

        chatScroll = new QScrollArea(ChatPanel);
        chatScroll->setObjectName(QStringLiteral("chatScroll"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(chatScroll->sizePolicy().hasHeightForWidth());
        chatScroll->setSizePolicy(sizePolicy1);
        chatScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        chatScroll->setWidgetResizable(true);
        scrollContent = new QWidget();
        scrollContent->setObjectName(QStringLiteral("scrollContent"));
        scrollContent->setGeometry(QRect(0, 0, 235, 188));
        scrollContent->setMinimumSize(QSize(0, 100));
        chatScroll->setWidget(scrollContent);

        gridLayout->addWidget(chatScroll, 1, 0, 1, 3);


        retranslateUi(ChatPanel);

        QMetaObject::connectSlotsByName(ChatPanel);
    } // setupUi

    void retranslateUi(QWidget *ChatPanel)
    {
        ChatPanel->setWindowTitle(QApplication::translate("ChatPanel", "Form", 0));
        iconLabel->setText(QString());
#ifndef QT_NO_ACCESSIBILITY
        chatText->setAccessibleDescription(QApplication::translate("ChatPanel", "Type your text to sent on the chat here", 0));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_TOOLTIP
        buttonClear->setToolTip(QApplication::translate("ChatPanel", "clear the messages ...", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        buttonClear->setAccessibleDescription(QApplication::translate("ChatPanel", "Press that button to clear your message", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonClear->setText(QString());
#ifndef QT_NO_ACCESSIBILITY
        chatScroll->setAccessibleDescription(QApplication::translate("ChatPanel", "This is the chat scroll area", 0));
#endif // QT_NO_ACCESSIBILITY
    } // retranslateUi

};

namespace Ui {
    class ChatPanel: public Ui_ChatPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATPANEL_H
