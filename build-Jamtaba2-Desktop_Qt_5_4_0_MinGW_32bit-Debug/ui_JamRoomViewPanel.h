/********************************************************************************
** Form generated from reading UI file 'JamRoomViewPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JAMROOMVIEWPANEL_H
#define UI_JAMROOMVIEWPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
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
    QGridLayout *gridLayout;
    QLabel *labelRoomStatus;
    QPushButton *buttonListen;
    QPushButton *buttonEnter;
    QLabel *labelName;
    QFrame *line;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QWidget *usersPanel;
    QVBoxLayout *verticalLayout_2;
    WavePeakPanel *wavePeakPanel;
    QLabel *labelRoomType;

    void setupUi(QWidget *RoomViewPanel)
    {
        if (RoomViewPanel->objectName().isEmpty())
            RoomViewPanel->setObjectName(QStringLiteral("RoomViewPanel"));
        RoomViewPanel->resize(634, 673);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RoomViewPanel->sizePolicy().hasHeightForWidth());
        RoomViewPanel->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(RoomViewPanel);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        panelTop = new QWidget(RoomViewPanel);
        panelTop->setObjectName(QStringLiteral("panelTop"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(panelTop->sizePolicy().hasHeightForWidth());
        panelTop->setSizePolicy(sizePolicy1);
        panelTop->setMinimumSize(QSize(0, 40));
        gridLayout = new QGridLayout(panelTop);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(5);
        gridLayout->setVerticalSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        labelRoomStatus = new QLabel(panelTop);
        labelRoomStatus->setObjectName(QStringLiteral("labelRoomStatus"));

        gridLayout->addWidget(labelRoomStatus, 2, 0, 1, 1);

        buttonListen = new QPushButton(panelTop);
        buttonListen->setObjectName(QStringLiteral("buttonListen"));
        buttonListen->setMaximumSize(QSize(50, 16777215));
        buttonListen->setCheckable(true);

        gridLayout->addWidget(buttonListen, 2, 2, 1, 1);

        buttonEnter = new QPushButton(panelTop);
        buttonEnter->setObjectName(QStringLiteral("buttonEnter"));
        buttonEnter->setMaximumSize(QSize(50, 16777215));

        gridLayout->addWidget(buttonEnter, 2, 3, 1, 1);

        labelName = new QLabel(panelTop);
        labelName->setObjectName(QStringLiteral("labelName"));
        labelName->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(labelName, 1, 0, 1, 4);


        verticalLayout->addWidget(panelTop);

        line = new QFrame(RoomViewPanel);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        widget_2 = new QWidget(RoomViewPanel);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        usersPanel = new QWidget(widget_2);
        usersPanel->setObjectName(QStringLiteral("usersPanel"));
        QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(usersPanel->sizePolicy().hasHeightForWidth());
        usersPanel->setSizePolicy(sizePolicy2);
        usersPanel->setMinimumSize(QSize(150, 0));
        verticalLayout_2 = new QVBoxLayout(usersPanel);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));

        horizontalLayout->addWidget(usersPanel);

        wavePeakPanel = new WavePeakPanel(widget_2);
        wavePeakPanel->setObjectName(QStringLiteral("wavePeakPanel"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(wavePeakPanel->sizePolicy().hasHeightForWidth());
        wavePeakPanel->setSizePolicy(sizePolicy3);

        horizontalLayout->addWidget(wavePeakPanel);

        labelRoomType = new QLabel(widget_2);
        labelRoomType->setObjectName(QStringLiteral("labelRoomType"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(labelRoomType->sizePolicy().hasHeightForWidth());
        labelRoomType->setSizePolicy(sizePolicy4);
        labelRoomType->setAlignment(Qt::AlignBottom|Qt::AlignRight|Qt::AlignTrailing);

        horizontalLayout->addWidget(labelRoomType);


        verticalLayout->addWidget(widget_2);


        retranslateUi(RoomViewPanel);

        QMetaObject::connectSlotsByName(RoomViewPanel);
    } // setupUi

    void retranslateUi(QWidget *RoomViewPanel)
    {
        RoomViewPanel->setWindowTitle(QApplication::translate("RoomViewPanel", "Form", 0));
        labelRoomStatus->setText(QApplication::translate("RoomViewPanel", "testee", 0));
        buttonListen->setText(QApplication::translate("RoomViewPanel", "listen", 0));
        buttonEnter->setText(QApplication::translate("RoomViewPanel", "enter", 0));
        labelName->setText(QApplication::translate("RoomViewPanel", "Name", 0));
        labelRoomType->setText(QApplication::translate("RoomViewPanel", "Ninjam", 0));
    } // retranslateUi

};

namespace Ui {
    class RoomViewPanel: public Ui_RoomViewPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JAMROOMVIEWPANEL_H
