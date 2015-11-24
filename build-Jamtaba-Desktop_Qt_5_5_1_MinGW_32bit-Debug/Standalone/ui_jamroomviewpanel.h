/********************************************************************************
** Form generated from reading UI file 'jamroomviewpanel.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JAMROOMVIEWPANEL_H
#define UI_JAMROOMVIEWPANEL_H

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
#include "widgets/WavePeakPanel.h"

QT_BEGIN_NAMESPACE

class Ui_RoomViewPanel
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *panelTop;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelName;
    QLabel *labelRoomStatus;
    QPushButton *buttonListen;
    QPushButton *buttonEnter;
    QWidget *content;
    QHBoxLayout *horizontalLayout;
    QWidget *usersPanel;
    QVBoxLayout *verticalLayout_2;
    WavePeakPanel *wavePeakPanel;

    void setupUi(QWidget *RoomViewPanel)
    {
        if (RoomViewPanel->objectName().isEmpty())
            RoomViewPanel->setObjectName(QStringLiteral("RoomViewPanel"));
        RoomViewPanel->resize(634, 224);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RoomViewPanel->sizePolicy().hasHeightForWidth());
        RoomViewPanel->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(RoomViewPanel);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        panelTop = new QWidget(RoomViewPanel);
        panelTop->setObjectName(QStringLiteral("panelTop"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(panelTop->sizePolicy().hasHeightForWidth());
        panelTop->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(panelTop);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(6, 6, 6, 6);
        labelName = new QLabel(panelTop);
        labelName->setObjectName(QStringLiteral("labelName"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelName->sizePolicy().hasHeightForWidth());
        labelName->setSizePolicy(sizePolicy2);
        labelName->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labelName);

        labelRoomStatus = new QLabel(panelTop);
        labelRoomStatus->setObjectName(QStringLiteral("labelRoomStatus"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(labelRoomStatus->sizePolicy().hasHeightForWidth());
        labelRoomStatus->setSizePolicy(sizePolicy3);
        labelRoomStatus->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labelRoomStatus);

        buttonListen = new QPushButton(panelTop);
        buttonListen->setObjectName(QStringLiteral("buttonListen"));
        buttonListen->setCheckable(true);

        horizontalLayout_2->addWidget(buttonListen);

        buttonEnter = new QPushButton(panelTop);
        buttonEnter->setObjectName(QStringLiteral("buttonEnter"));
        buttonEnter->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_2->addWidget(buttonEnter);


        verticalLayout->addWidget(panelTop);

        content = new QWidget(RoomViewPanel);
        content->setObjectName(QStringLiteral("content"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(1);
        sizePolicy4.setVerticalStretch(1);
        sizePolicy4.setHeightForWidth(content->sizePolicy().hasHeightForWidth());
        content->setSizePolicy(sizePolicy4);
        horizontalLayout = new QHBoxLayout(content);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(12, 3, 12, 0);
        usersPanel = new QWidget(content);
        usersPanel->setObjectName(QStringLiteral("usersPanel"));
        QSizePolicy sizePolicy5(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(usersPanel->sizePolicy().hasHeightForWidth());
        usersPanel->setSizePolicy(sizePolicy5);
        usersPanel->setMinimumSize(QSize(150, 0));
        verticalLayout_2 = new QVBoxLayout(usersPanel);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));

        horizontalLayout->addWidget(usersPanel);

        wavePeakPanel = new WavePeakPanel(content);
        wavePeakPanel->setObjectName(QStringLiteral("wavePeakPanel"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(1);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(wavePeakPanel->sizePolicy().hasHeightForWidth());
        wavePeakPanel->setSizePolicy(sizePolicy6);
        wavePeakPanel->setMinimumSize(QSize(0, 30));

        horizontalLayout->addWidget(wavePeakPanel);


        verticalLayout->addWidget(content);


        retranslateUi(RoomViewPanel);

        QMetaObject::connectSlotsByName(RoomViewPanel);
    } // setupUi

    void retranslateUi(QWidget *RoomViewPanel)
    {
        RoomViewPanel->setWindowTitle(QApplication::translate("RoomViewPanel", "Form", 0));
#ifndef QT_NO_ACCESSIBILITY
        labelName->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        labelName->setAccessibleDescription(QApplication::translate("RoomViewPanel", "This is the server name", 0));
#endif // QT_NO_ACCESSIBILITY
        labelName->setText(QApplication::translate("RoomViewPanel", "Name", 0));
#ifndef QT_NO_ACCESSIBILITY
        labelRoomStatus->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        labelRoomStatus->setAccessibleDescription(QApplication::translate("RoomViewPanel", "This is the status of this room", 0));
#endif // QT_NO_ACCESSIBILITY
        labelRoomStatus->setText(QApplication::translate("RoomViewPanel", "testee", 0));
#ifndef QT_NO_ACCESSIBILITY
        buttonListen->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        buttonListen->setAccessibleDescription(QApplication::translate("RoomViewPanel", "Click that button to listen that room", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonListen->setText(QApplication::translate("RoomViewPanel", "listen", 0));
#ifndef QT_NO_ACCESSIBILITY
        buttonEnter->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        buttonEnter->setAccessibleDescription(QApplication::translate("RoomViewPanel", "Click that button to enter in this room", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonEnter->setText(QApplication::translate("RoomViewPanel", "enter", 0));
#ifndef QT_NO_ACCESSIBILITY
        usersPanel->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        usersPanel->setAccessibleDescription(QApplication::translate("RoomViewPanel", "That panel shows the musicians actually in that room", 0));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        wavePeakPanel->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        wavePeakPanel->setAccessibleDescription(QApplication::translate("RoomViewPanel", "That zone displays the waveform actually played", 0));
#endif // QT_NO_ACCESSIBILITY
    } // retranslateUi

};

namespace Ui {
    class RoomViewPanel: public Ui_RoomViewPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JAMROOMVIEWPANEL_H
