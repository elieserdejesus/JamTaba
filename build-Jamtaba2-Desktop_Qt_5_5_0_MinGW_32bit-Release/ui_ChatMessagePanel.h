/********************************************************************************
** Form generated from reading UI file 'ChatMessagePanel.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATMESSAGEPANEL_H
#define UI_CHATMESSAGEPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatMessagePanel
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *labelUserName;
    QLabel *labelMessage;

    void setupUi(QWidget *ChatMessagePanel)
    {
        if (ChatMessagePanel->objectName().isEmpty())
            ChatMessagePanel->setObjectName(QStringLiteral("ChatMessagePanel"));
        ChatMessagePanel->resize(308, 135);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ChatMessagePanel->sizePolicy().hasHeightForWidth());
        ChatMessagePanel->setSizePolicy(sizePolicy);
        ChatMessagePanel->setAutoFillBackground(false);
        horizontalLayout = new QHBoxLayout(ChatMessagePanel);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        labelUserName = new QLabel(ChatMessagePanel);
        labelUserName->setObjectName(QStringLiteral("labelUserName"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelUserName->sizePolicy().hasHeightForWidth());
        labelUserName->setSizePolicy(sizePolicy1);
        labelUserName->setLayoutDirection(Qt::LeftToRight);
        labelUserName->setAutoFillBackground(false);
        labelUserName->setScaledContents(false);
        labelUserName->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        horizontalLayout->addWidget(labelUserName, 0, Qt::AlignTop);

        labelMessage = new QLabel(ChatMessagePanel);
        labelMessage->setObjectName(QStringLiteral("labelMessage"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelMessage->sizePolicy().hasHeightForWidth());
        labelMessage->setSizePolicy(sizePolicy2);
        labelMessage->setTextFormat(Qt::RichText);
        labelMessage->setWordWrap(true);
        labelMessage->setOpenExternalLinks(true);
        labelMessage->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse|Qt::TextBrowserInteraction|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        horizontalLayout->addWidget(labelMessage);

        labelMessage->raise();
        labelUserName->raise();

        retranslateUi(ChatMessagePanel);

        QMetaObject::connectSlotsByName(ChatMessagePanel);
    } // setupUi

    void retranslateUi(QWidget *ChatMessagePanel)
    {
        ChatMessagePanel->setWindowTitle(QApplication::translate("ChatMessagePanel", "Form", 0));
        labelUserName->setText(QApplication::translate("ChatMessagePanel", "user name", 0));
        labelMessage->setText(QApplication::translate("ChatMessagePanel", "message", 0));
    } // retranslateUi

};

namespace Ui {
    class ChatMessagePanel: public Ui_ChatMessagePanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATMESSAGEPANEL_H
