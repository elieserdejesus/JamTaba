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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "NinjamPanel.h"

QT_BEGIN_NAMESPACE

class Ui_NinjamRoomWindow
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelRoomName;
    QPushButton *licenceButton;
    QFrame *line;
    QWidget *content;
    QVBoxLayout *verticalLayout_2;
    NinjamPanel *topPanel;
    QScrollArea *scrollArea;
    QWidget *tracksPanel;
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *NinjamRoomWindow)
    {
        if (NinjamRoomWindow->objectName().isEmpty())
            NinjamRoomWindow->setObjectName(QStringLiteral("NinjamRoomWindow"));
        NinjamRoomWindow->resize(747, 474);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(NinjamRoomWindow->sizePolicy().hasHeightForWidth());
        NinjamRoomWindow->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(NinjamRoomWindow);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget = new QWidget(NinjamRoomWindow);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        labelRoomName = new QLabel(widget);
        labelRoomName->setObjectName(QStringLiteral("labelRoomName"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelRoomName->sizePolicy().hasHeightForWidth());
        labelRoomName->setSizePolicy(sizePolicy1);
        labelRoomName->setText(QStringLiteral("TextLabel"));
        labelRoomName->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(labelRoomName);

        licenceButton = new QPushButton(widget);
        licenceButton->setObjectName(QStringLiteral("licenceButton"));
        licenceButton->setFlat(false);

        horizontalLayout_2->addWidget(licenceButton);


        verticalLayout->addWidget(widget);

        line = new QFrame(NinjamRoomWindow);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        content = new QWidget(NinjamRoomWindow);
        content->setObjectName(QStringLiteral("content"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(content->sizePolicy().hasHeightForWidth());
        content->setSizePolicy(sizePolicy2);
        verticalLayout_2 = new QVBoxLayout(content);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        topPanel = new NinjamPanel(content);
        topPanel->setObjectName(QStringLiteral("topPanel"));

        verticalLayout_2->addWidget(topPanel);

        scrollArea = new QScrollArea(content);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(true);
        tracksPanel = new QWidget();
        tracksPanel->setObjectName(QStringLiteral("tracksPanel"));
        tracksPanel->setGeometry(QRect(0, 0, 709, 382));
        horizontalLayout = new QHBoxLayout(tracksPanel);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 6, 0, 6);
        scrollArea->setWidget(tracksPanel);

        verticalLayout_2->addWidget(scrollArea);


        verticalLayout->addWidget(content);


        retranslateUi(NinjamRoomWindow);

        QMetaObject::connectSlotsByName(NinjamRoomWindow);
    } // setupUi

    void retranslateUi(QWidget *NinjamRoomWindow)
    {
        NinjamRoomWindow->setWindowTitle(QApplication::translate("NinjamRoomWindow", "Form", 0));
#ifndef QT_NO_TOOLTIP
        licenceButton->setToolTip(QApplication::translate("NinjamRoomWindow", "read the server licence ...", 0));
#endif // QT_NO_TOOLTIP
        licenceButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class NinjamRoomWindow: public Ui_NinjamRoomWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NINJAMROOMWINDOW_H
