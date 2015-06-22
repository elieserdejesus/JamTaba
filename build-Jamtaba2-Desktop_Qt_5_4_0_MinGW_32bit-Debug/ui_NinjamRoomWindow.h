/********************************************************************************
** Form generated from reading UI file 'NinjamRoomWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NINJAMROOMWINDOW_H
#define UI_NINJAMROOMWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "NinjamPanel.h"

QT_BEGIN_NAMESPACE

class Ui_NinjamRoomWindow
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *labelRoomName;
    QFrame *line;
    QWidget *content;
    QVBoxLayout *verticalLayout_2;
    NinjamPanel *topPanel;
    QWidget *tracksPanel;
    QHBoxLayout *horizontalLayout_2;

    void setupUi(QWidget *NinjamRoomWindow)
    {
        if (NinjamRoomWindow->objectName().isEmpty())
            NinjamRoomWindow->setObjectName(QStringLiteral("NinjamRoomWindow"));
        NinjamRoomWindow->resize(747, 474);
        verticalLayout = new QVBoxLayout(NinjamRoomWindow);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        labelRoomName = new QLabel(NinjamRoomWindow);
        labelRoomName->setObjectName(QStringLiteral("labelRoomName"));
        labelRoomName->setText(QStringLiteral("TextLabel"));
        labelRoomName->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelRoomName);

        line = new QFrame(NinjamRoomWindow);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        content = new QWidget(NinjamRoomWindow);
        content->setObjectName(QStringLiteral("content"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(content->sizePolicy().hasHeightForWidth());
        content->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(content);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        topPanel = new NinjamPanel(content);
        topPanel->setObjectName(QStringLiteral("topPanel"));

        verticalLayout_2->addWidget(topPanel);

        tracksPanel = new QWidget(content);
        tracksPanel->setObjectName(QStringLiteral("tracksPanel"));
        sizePolicy.setHeightForWidth(tracksPanel->sizePolicy().hasHeightForWidth());
        tracksPanel->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(tracksPanel);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 6, 0, 0);

        verticalLayout_2->addWidget(tracksPanel);


        verticalLayout->addWidget(content);


        retranslateUi(NinjamRoomWindow);

        QMetaObject::connectSlotsByName(NinjamRoomWindow);
    } // setupUi

    void retranslateUi(QWidget *NinjamRoomWindow)
    {
        NinjamRoomWindow->setWindowTitle(QApplication::translate("NinjamRoomWindow", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class NinjamRoomWindow: public Ui_NinjamRoomWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NINJAMROOMWINDOW_H
