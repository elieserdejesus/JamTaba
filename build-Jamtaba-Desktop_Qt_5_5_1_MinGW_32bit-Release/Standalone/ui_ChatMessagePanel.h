/********************************************************************************
** Form generated from reading UI file 'ChatMessagePanel.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatMessagePanel
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLabel *labelUserName;
    QPushButton *translateButton;
    QLabel *labelMessage;

    void setupUi(QWidget *ChatMessagePanel)
    {
        if (ChatMessagePanel->objectName().isEmpty())
            ChatMessagePanel->setObjectName(QStringLiteral("ChatMessagePanel"));
        ChatMessagePanel->resize(310, 125);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ChatMessagePanel->sizePolicy().hasHeightForWidth());
        ChatMessagePanel->setSizePolicy(sizePolicy);
        ChatMessagePanel->setAutoFillBackground(false);
        horizontalLayout = new QHBoxLayout(ChatMessagePanel);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(ChatMessagePanel);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(2);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        labelUserName = new QLabel(widget);
        labelUserName->setObjectName(QStringLiteral("labelUserName"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelUserName->sizePolicy().hasHeightForWidth());
        labelUserName->setSizePolicy(sizePolicy2);
        labelUserName->setLayoutDirection(Qt::LeftToRight);
        labelUserName->setAutoFillBackground(false);
        labelUserName->setScaledContents(false);
        labelUserName->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        verticalLayout->addWidget(labelUserName);

        translateButton = new QPushButton(widget);
        translateButton->setObjectName(QStringLiteral("translateButton"));
        QSizePolicy sizePolicy3(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(translateButton->sizePolicy().hasHeightForWidth());
        translateButton->setSizePolicy(sizePolicy3);
        translateButton->setCheckable(true);

        verticalLayout->addWidget(translateButton, 0, Qt::AlignRight|Qt::AlignBottom);


        horizontalLayout->addWidget(widget, 0, Qt::AlignTop);

        labelMessage = new QLabel(ChatMessagePanel);
        labelMessage->setObjectName(QStringLiteral("labelMessage"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(labelMessage->sizePolicy().hasHeightForWidth());
        labelMessage->setSizePolicy(sizePolicy4);
        labelMessage->setTextFormat(Qt::RichText);
        labelMessage->setScaledContents(false);
        labelMessage->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        labelMessage->setWordWrap(true);
        labelMessage->setOpenExternalLinks(true);
        labelMessage->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse|Qt::TextBrowserInteraction|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        horizontalLayout->addWidget(labelMessage);


        retranslateUi(ChatMessagePanel);

        QMetaObject::connectSlotsByName(ChatMessagePanel);
    } // setupUi

    void retranslateUi(QWidget *ChatMessagePanel)
    {
        ChatMessagePanel->setWindowTitle(QApplication::translate("ChatMessagePanel", "Form", 0));
        labelUserName->setText(QApplication::translate("ChatMessagePanel", "user name", 0));
#ifndef QT_NO_TOOLTIP
        translateButton->setToolTip(QApplication::translate("ChatMessagePanel", "translate ...", 0));
#endif // QT_NO_TOOLTIP
        translateButton->setText(QApplication::translate("ChatMessagePanel", "T", 0));
        labelMessage->setText(QApplication::translate("ChatMessagePanel", "big chat message with many lines to test the behavior", 0));
    } // retranslateUi

};

namespace Ui {
    class ChatMessagePanel: public Ui_ChatMessagePanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATMESSAGEPANEL_H
