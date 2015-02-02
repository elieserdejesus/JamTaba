/********************************************************************************
** Form generated from reading UI file 'mainframe.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFRAME_H
#define UI_MAINFRAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "TrackView.h"

QT_BEGIN_NAMESPACE

class Ui_MainFrameClass
{
public:
    QAction *actionAudio;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *leftPanel;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QFrame *line;
    Gui::TrackView *widget_2;
    QWidget *contentPanel;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QFrame *line_2;
    QWidget *widget_3;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QWidget *chatPanel;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QFrame *line_3;
    QWidget *widget_4;
    QWidget *onlineUsersPanel;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_4;
    QFrame *line_4;
    QWidget *widget_5;
    QMenuBar *menuBar;
    QMenu *menuAudio;

    void setupUi(QMainWindow *MainFrameClass)
    {
        if (MainFrameClass->objectName().isEmpty())
            MainFrameClass->setObjectName(QStringLiteral("MainFrameClass"));
        MainFrameClass->resize(1160, 750);
        MainFrameClass->setMinimumSize(QSize(1160, 750));
        MainFrameClass->setWindowTitle(QStringLiteral("Jamtaba"));
        actionAudio = new QAction(MainFrameClass);
        actionAudio->setObjectName(QStringLiteral("actionAudio"));
        centralWidget = new QWidget(MainFrameClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        leftPanel = new QWidget(centralWidget);
        leftPanel->setObjectName(QStringLiteral("leftPanel"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(leftPanel->sizePolicy().hasHeightForWidth());
        leftPanel->setSizePolicy(sizePolicy);
        leftPanel->setMinimumSize(QSize(100, 0));
        verticalLayout_2 = new QVBoxLayout(leftPanel);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label = new QLabel(leftPanel);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_2->addWidget(label);

        line = new QFrame(leftPanel);
        line->setObjectName(QStringLiteral("line"));
        line->setAutoFillBackground(false);
        line->setFrameShadow(QFrame::Sunken);
        line->setFrameShape(QFrame::HLine);

        verticalLayout_2->addWidget(line);

        widget_2 = new Gui::TrackView(leftPanel);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(widget_2);


        horizontalLayout->addWidget(leftPanel);

        contentPanel = new QWidget(centralWidget);
        contentPanel->setObjectName(QStringLiteral("contentPanel"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(3);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(contentPanel->sizePolicy().hasHeightForWidth());
        contentPanel->setSizePolicy(sizePolicy2);
        verticalLayout_3 = new QVBoxLayout(contentPanel);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_2 = new QLabel(contentPanel);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_3->addWidget(label_2);

        line_2 = new QFrame(contentPanel);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setAutoFillBackground(false);
        line_2->setFrameShadow(QFrame::Sunken);
        line_2->setFrameShape(QFrame::HLine);

        verticalLayout_3->addWidget(line_2);

        widget_3 = new QWidget(contentPanel);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        sizePolicy1.setHeightForWidth(widget_3->sizePolicy().hasHeightForWidth());
        widget_3->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(widget_3);


        horizontalLayout->addWidget(contentPanel);

        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy3);
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        chatPanel = new QWidget(widget);
        chatPanel->setObjectName(QStringLiteral("chatPanel"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(3);
        sizePolicy4.setHeightForWidth(chatPanel->sizePolicy().hasHeightForWidth());
        chatPanel->setSizePolicy(sizePolicy4);
        verticalLayout_4 = new QVBoxLayout(chatPanel);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_3 = new QLabel(chatPanel);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_4->addWidget(label_3);

        line_3 = new QFrame(chatPanel);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout_4->addWidget(line_3);

        widget_4 = new QWidget(chatPanel);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        sizePolicy1.setHeightForWidth(widget_4->sizePolicy().hasHeightForWidth());
        widget_4->setSizePolicy(sizePolicy1);

        verticalLayout_4->addWidget(widget_4);


        verticalLayout->addWidget(chatPanel);

        onlineUsersPanel = new QWidget(widget);
        onlineUsersPanel->setObjectName(QStringLiteral("onlineUsersPanel"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(2);
        sizePolicy5.setHeightForWidth(onlineUsersPanel->sizePolicy().hasHeightForWidth());
        onlineUsersPanel->setSizePolicy(sizePolicy5);
        verticalLayout_5 = new QVBoxLayout(onlineUsersPanel);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        label_4 = new QLabel(onlineUsersPanel);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_5->addWidget(label_4);

        line_4 = new QFrame(onlineUsersPanel);
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        verticalLayout_5->addWidget(line_4);

        widget_5 = new QWidget(onlineUsersPanel);
        widget_5->setObjectName(QStringLiteral("widget_5"));
        sizePolicy1.setHeightForWidth(widget_5->sizePolicy().hasHeightForWidth());
        widget_5->setSizePolicy(sizePolicy1);

        verticalLayout_5->addWidget(widget_5);


        verticalLayout->addWidget(onlineUsersPanel);


        horizontalLayout->addWidget(widget);

        MainFrameClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainFrameClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1160, 21));
        menuAudio = new QMenu(menuBar);
        menuAudio->setObjectName(QStringLiteral("menuAudio"));
        MainFrameClass->setMenuBar(menuBar);

        menuBar->addAction(menuAudio->menuAction());
        menuAudio->addAction(actionAudio);

        retranslateUi(MainFrameClass);

        QMetaObject::connectSlotsByName(MainFrameClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainFrameClass)
    {
        actionAudio->setText(QApplication::translate("MainFrameClass", "Audio", 0));
        label->setText(QApplication::translate("MainFrameClass", "TextLabel", 0));
        label_2->setText(QApplication::translate("MainFrameClass", "TextLabel", 0));
        label_3->setText(QApplication::translate("MainFrameClass", "TextLabel", 0));
        label_4->setText(QApplication::translate("MainFrameClass", "TextLabel", 0));
        menuAudio->setTitle(QApplication::translate("MainFrameClass", "Preferences", 0));
        Q_UNUSED(MainFrameClass);
    } // retranslateUi

};

namespace Ui {
    class MainFrameClass: public Ui_MainFrameClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFRAME_H
