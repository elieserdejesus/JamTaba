/********************************************************************************
** Form generated from reading UI file 'MainFrame.ui'
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
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainFrameClass
{
public:
    QAction *actionTest;
    QAction *menuAudioPreferences;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *leftPanel;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QFrame *line;
    QWidget *localTracksWidget;
    QHBoxLayout *localTracksLayout;
    QWidget *contentPanel;
    QVBoxLayout *verticalLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_5;
    QScrollArea *allRoomsScroll;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_6;
    QWidget *allRoomsContent;
    QWidget *chatPanel_2;
    QVBoxLayout *verticalLayout;
    QWidget *chatPanel;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QFrame *line_3;
    QWidget *widget_4;
    QMenuBar *menuBar;
    QMenu *menuPreferences;

    void setupUi(QMainWindow *MainFrameClass)
    {
        if (MainFrameClass->objectName().isEmpty())
            MainFrameClass->setObjectName(QStringLiteral("MainFrameClass"));
        MainFrameClass->resize(1160, 750);
        MainFrameClass->setMinimumSize(QSize(1160, 750));
        MainFrameClass->setWindowTitle(QStringLiteral("Jamtaba"));
        actionTest = new QAction(MainFrameClass);
        actionTest->setObjectName(QStringLiteral("actionTest"));
        menuAudioPreferences = new QAction(MainFrameClass);
        menuAudioPreferences->setObjectName(QStringLiteral("menuAudioPreferences"));
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

        localTracksWidget = new QWidget(leftPanel);
        localTracksWidget->setObjectName(QStringLiteral("localTracksWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(localTracksWidget->sizePolicy().hasHeightForWidth());
        localTracksWidget->setSizePolicy(sizePolicy1);
        localTracksLayout = new QHBoxLayout(localTracksWidget);
        localTracksLayout->setSpacing(6);
        localTracksLayout->setContentsMargins(11, 11, 11, 11);
        localTracksLayout->setObjectName(QStringLiteral("localTracksLayout"));
        localTracksLayout->setContentsMargins(0, 0, 0, 0);

        verticalLayout_2->addWidget(localTracksWidget);


        horizontalLayout->addWidget(leftPanel);

        contentPanel = new QWidget(centralWidget);
        contentPanel->setObjectName(QStringLiteral("contentPanel"));
        verticalLayout_3 = new QVBoxLayout(contentPanel);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        tabWidget = new QTabWidget(contentPanel);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideLeft);
        tabWidget->setDocumentMode(false);
        tabWidget->setTabsClosable(true);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_5 = new QVBoxLayout(tab);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        allRoomsScroll = new QScrollArea(tab);
        allRoomsScroll->setObjectName(QStringLiteral("allRoomsScroll"));
        allRoomsScroll->setStyleSheet(QStringLiteral(""));
        allRoomsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        allRoomsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        allRoomsScroll->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 689, 644));
        verticalLayout_6 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        allRoomsContent = new QWidget(scrollAreaWidgetContents);
        allRoomsContent->setObjectName(QStringLiteral("allRoomsContent"));
        QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(allRoomsContent->sizePolicy().hasHeightForWidth());
        allRoomsContent->setSizePolicy(sizePolicy2);

        verticalLayout_6->addWidget(allRoomsContent);

        allRoomsScroll->setWidget(scrollAreaWidgetContents);

        verticalLayout_5->addWidget(allRoomsScroll);

        tabWidget->addTab(tab, QString());

        verticalLayout_3->addWidget(tabWidget);


        horizontalLayout->addWidget(contentPanel);

        chatPanel_2 = new QWidget(centralWidget);
        chatPanel_2->setObjectName(QStringLiteral("chatPanel_2"));
        sizePolicy.setHeightForWidth(chatPanel_2->sizePolicy().hasHeightForWidth());
        chatPanel_2->setSizePolicy(sizePolicy);
        chatPanel_2->setMinimumSize(QSize(280, 0));
        verticalLayout = new QVBoxLayout(chatPanel_2);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        chatPanel = new QWidget(chatPanel_2);
        chatPanel->setObjectName(QStringLiteral("chatPanel"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(chatPanel->sizePolicy().hasHeightForWidth());
        chatPanel->setSizePolicy(sizePolicy3);
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


        horizontalLayout->addWidget(chatPanel_2);

        MainFrameClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainFrameClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1160, 21));
        menuPreferences = new QMenu(menuBar);
        menuPreferences->setObjectName(QStringLiteral("menuPreferences"));
        MainFrameClass->setMenuBar(menuBar);

        menuBar->addAction(menuPreferences->menuAction());
        menuPreferences->addAction(menuAudioPreferences);

        retranslateUi(MainFrameClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainFrameClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainFrameClass)
    {
        actionTest->setText(QApplication::translate("MainFrameClass", "test", 0));
        menuAudioPreferences->setText(QApplication::translate("MainFrameClass", "Audio...", 0));
        label->setText(QApplication::translate("MainFrameClass", "Your Controls", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainFrameClass", "Rooms to play", 0));
        label_3->setText(QApplication::translate("MainFrameClass", "Chat", 0));
        menuPreferences->setTitle(QApplication::translate("MainFrameClass", "Preferences", 0));
        Q_UNUSED(MainFrameClass);
    } // retranslateUi

};

namespace Ui {
    class MainFrameClass: public Ui_MainFrameClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFRAME_H
