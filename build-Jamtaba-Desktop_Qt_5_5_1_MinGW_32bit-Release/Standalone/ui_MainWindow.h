/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "CustomTabWidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainFrameClass
{
public:
    QAction *actionTest;
    QAction *actionAudioPreferences;
    QAction *actionMidiPreferences;
    QAction *actionVstPreferences;
    QAction *actionRecording;
    QAction *actionPrivate_Server;
    QAction *actionNinjam_community_forum;
    QAction *actionNinjam_Official_Site;
    QAction *actionReportBugs;
    QAction *actionWiki;
    QAction *actionUsersManual;
    QAction *actionFullView;
    QAction *actionMiniView;
    QAction *actionQuit;
    QAction *actionFullscreenMode;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *leftPanel;
    QVBoxLayout *verticalLayout_2;
    QWidget *topWidget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelSectionTitle;
    QPushButton *localControlsCollapseButton;
    QScrollArea *scrollArea;
    QWidget *localTracksWidget;
    QHBoxLayout *localTracksLayout;
    QWidget *contentPanel;
    QVBoxLayout *verticalLayout_3;
    CustomTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *tabLayout;
    QScrollArea *allRoomsScroll;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_6;
    QWidget *allRoomsContent;
    QWidget *chatArea;
    QVBoxLayout *verticalLayout;
    QTabWidget *chatTabWidget;
    QMenuBar *menuBar;
    QMenu *menuPreferences;
    QMenu *menuNinjam;
    QMenu *menuHelp;
    QMenu *menuViewMode;

    void setupUi(QMainWindow *MainFrameClass)
    {
        if (MainFrameClass->objectName().isEmpty())
            MainFrameClass->setObjectName(QStringLiteral("MainFrameClass"));
        MainFrameClass->resize(1160, 750);
        MainFrameClass->setWindowTitle(QStringLiteral("Jamtaba"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/jt_0001.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainFrameClass->setWindowIcon(icon);
        actionTest = new QAction(MainFrameClass);
        actionTest->setObjectName(QStringLiteral("actionTest"));
        actionAudioPreferences = new QAction(MainFrameClass);
        actionAudioPreferences->setObjectName(QStringLiteral("actionAudioPreferences"));
        actionMidiPreferences = new QAction(MainFrameClass);
        actionMidiPreferences->setObjectName(QStringLiteral("actionMidiPreferences"));
        actionVstPreferences = new QAction(MainFrameClass);
        actionVstPreferences->setObjectName(QStringLiteral("actionVstPreferences"));
        actionRecording = new QAction(MainFrameClass);
        actionRecording->setObjectName(QStringLiteral("actionRecording"));
        actionPrivate_Server = new QAction(MainFrameClass);
        actionPrivate_Server->setObjectName(QStringLiteral("actionPrivate_Server"));
        actionNinjam_community_forum = new QAction(MainFrameClass);
        actionNinjam_community_forum->setObjectName(QStringLiteral("actionNinjam_community_forum"));
        actionNinjam_Official_Site = new QAction(MainFrameClass);
        actionNinjam_Official_Site->setObjectName(QStringLiteral("actionNinjam_Official_Site"));
        actionReportBugs = new QAction(MainFrameClass);
        actionReportBugs->setObjectName(QStringLiteral("actionReportBugs"));
        actionWiki = new QAction(MainFrameClass);
        actionWiki->setObjectName(QStringLiteral("actionWiki"));
        actionUsersManual = new QAction(MainFrameClass);
        actionUsersManual->setObjectName(QStringLiteral("actionUsersManual"));
        actionFullView = new QAction(MainFrameClass);
        actionFullView->setObjectName(QStringLiteral("actionFullView"));
        actionFullView->setCheckable(true);
        actionMiniView = new QAction(MainFrameClass);
        actionMiniView->setObjectName(QStringLiteral("actionMiniView"));
        actionMiniView->setCheckable(true);
        actionQuit = new QAction(MainFrameClass);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionFullscreenMode = new QAction(MainFrameClass);
        actionFullscreenMode->setObjectName(QStringLiteral("actionFullscreenMode"));
        actionFullscreenMode->setCheckable(true);
        centralWidget = new QWidget(MainFrameClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(12);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(3, 3, 3, 3);
        leftPanel = new QWidget(centralWidget);
        leftPanel->setObjectName(QStringLiteral("leftPanel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(leftPanel->sizePolicy().hasHeightForWidth());
        leftPanel->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(leftPanel);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(6, 3, 6, 6);
        topWidget = new QWidget(leftPanel);
        topWidget->setObjectName(QStringLiteral("topWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(topWidget->sizePolicy().hasHeightForWidth());
        topWidget->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(topWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        labelSectionTitle = new QLabel(topWidget);
        labelSectionTitle->setObjectName(QStringLiteral("labelSectionTitle"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelSectionTitle->sizePolicy().hasHeightForWidth());
        labelSectionTitle->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(labelSectionTitle);

        localControlsCollapseButton = new QPushButton(topWidget);
        localControlsCollapseButton->setObjectName(QStringLiteral("localControlsCollapseButton"));
        localControlsCollapseButton->setCheckable(true);

        horizontalLayout_2->addWidget(localControlsCollapseButton);


        verticalLayout_2->addWidget(topWidget);

        scrollArea = new QScrollArea(leftPanel);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        localTracksWidget = new QWidget();
        localTracksWidget->setObjectName(QStringLiteral("localTracksWidget"));
        localTracksWidget->setGeometry(QRect(0, 0, 99, 683));
        localTracksLayout = new QHBoxLayout(localTracksWidget);
        localTracksLayout->setSpacing(3);
        localTracksLayout->setContentsMargins(11, 11, 11, 11);
        localTracksLayout->setObjectName(QStringLiteral("localTracksLayout"));
        localTracksLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea->setWidget(localTracksWidget);

        verticalLayout_2->addWidget(scrollArea);


        horizontalLayout->addWidget(leftPanel);

        contentPanel = new QWidget(centralWidget);
        contentPanel->setObjectName(QStringLiteral("contentPanel"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(2);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(contentPanel->sizePolicy().hasHeightForWidth());
        contentPanel->setSizePolicy(sizePolicy3);
        verticalLayout_3 = new QVBoxLayout(contentPanel);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        tabWidget = new CustomTabWidget(contentPanel);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideLeft);
        tabWidget->setDocumentMode(false);
        tabWidget->setTabsClosable(true);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tabLayout = new QVBoxLayout(tab);
        tabLayout->setSpacing(0);
        tabLayout->setContentsMargins(11, 11, 11, 11);
        tabLayout->setObjectName(QStringLiteral("tabLayout"));
        allRoomsScroll = new QScrollArea(tab);
        allRoomsScroll->setObjectName(QStringLiteral("allRoomsScroll"));
        allRoomsScroll->setStyleSheet(QStringLiteral(""));
        allRoomsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        allRoomsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        allRoomsScroll->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 711, 674));
        verticalLayout_6 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_6->setSpacing(50);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 3, 0);
        allRoomsContent = new QWidget(scrollAreaWidgetContents);
        allRoomsContent->setObjectName(QStringLiteral("allRoomsContent"));
        QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(allRoomsContent->sizePolicy().hasHeightForWidth());
        allRoomsContent->setSizePolicy(sizePolicy4);

        verticalLayout_6->addWidget(allRoomsContent);

        allRoomsScroll->setWidget(scrollAreaWidgetContents);

        tabLayout->addWidget(allRoomsScroll);

        tabWidget->addTab(tab, QString());

        verticalLayout_3->addWidget(tabWidget);


        horizontalLayout->addWidget(contentPanel);

        chatArea = new QWidget(centralWidget);
        chatArea->setObjectName(QStringLiteral("chatArea"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(1);
        sizePolicy5.setHeightForWidth(chatArea->sizePolicy().hasHeightForWidth());
        chatArea->setSizePolicy(sizePolicy5);
        chatArea->setMinimumSize(QSize(280, 0));
        verticalLayout = new QVBoxLayout(chatArea);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        chatTabWidget = new QTabWidget(chatArea);
        chatTabWidget->setObjectName(QStringLiteral("chatTabWidget"));
        QSizePolicy sizePolicy6(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(chatTabWidget->sizePolicy().hasHeightForWidth());
        chatTabWidget->setSizePolicy(sizePolicy6);

        verticalLayout->addWidget(chatTabWidget);


        horizontalLayout->addWidget(chatArea);

        MainFrameClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainFrameClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1160, 21));
        menuPreferences = new QMenu(menuBar);
        menuPreferences->setObjectName(QStringLiteral("menuPreferences"));
        menuNinjam = new QMenu(menuBar);
        menuNinjam->setObjectName(QStringLiteral("menuNinjam"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuViewMode = new QMenu(menuBar);
        menuViewMode->setObjectName(QStringLiteral("menuViewMode"));
        MainFrameClass->setMenuBar(menuBar);

        menuBar->addAction(menuPreferences->menuAction());
        menuBar->addAction(menuViewMode->menuAction());
        menuBar->addAction(menuNinjam->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuPreferences->addAction(actionAudioPreferences);
        menuPreferences->addAction(actionMidiPreferences);
        menuPreferences->addAction(actionVstPreferences);
        menuPreferences->addAction(actionRecording);
        menuPreferences->addSeparator();
        menuPreferences->addAction(actionQuit);
        menuNinjam->addAction(actionPrivate_Server);
        menuNinjam->addAction(actionNinjam_community_forum);
        menuNinjam->addAction(actionNinjam_Official_Site);
        menuHelp->addAction(actionWiki);
        menuHelp->addAction(actionUsersManual);
        menuHelp->addAction(actionReportBugs);
        menuViewMode->addAction(actionFullView);
        menuViewMode->addAction(actionMiniView);
        menuViewMode->addSeparator();
        menuViewMode->addAction(actionFullscreenMode);

        retranslateUi(MainFrameClass);
        QObject::connect(actionFullscreenMode, SIGNAL(triggered()), MainFrameClass, SLOT(showFullScreen()));

        tabWidget->setCurrentIndex(0);
        chatTabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainFrameClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainFrameClass)
    {
        actionTest->setText(QApplication::translate("MainFrameClass", "test", 0));
        actionAudioPreferences->setText(QApplication::translate("MainFrameClass", "Audio ...", 0));
        actionAudioPreferences->setShortcut(QApplication::translate("MainFrameClass", "F5", 0));
        actionMidiPreferences->setText(QApplication::translate("MainFrameClass", "Midi ...", 0));
        actionMidiPreferences->setShortcut(QApplication::translate("MainFrameClass", "F6", 0));
        actionVstPreferences->setText(QApplication::translate("MainFrameClass", "VST Plugins ...", 0));
        actionVstPreferences->setShortcut(QApplication::translate("MainFrameClass", "F7", 0));
        actionRecording->setText(QApplication::translate("MainFrameClass", "Recording ...", 0));
        actionRecording->setShortcut(QApplication::translate("MainFrameClass", "F8", 0));
        actionPrivate_Server->setText(QApplication::translate("MainFrameClass", "Private Server ...", 0));
        actionNinjam_community_forum->setText(QApplication::translate("MainFrameClass", "Ninjam community (Ninbot) ...", 0));
        actionNinjam_Official_Site->setText(QApplication::translate("MainFrameClass", "Ninjam Official Site ...", 0));
        actionReportBugs->setText(QApplication::translate("MainFrameClass", "Report bugs or suggest improvements ...", 0));
        actionWiki->setText(QApplication::translate("MainFrameClass", "Wiki ...", 0));
        actionUsersManual->setText(QApplication::translate("MainFrameClass", "Users Manual ...", 0));
        actionUsersManual->setShortcut(QApplication::translate("MainFrameClass", "F1", 0));
        actionFullView->setText(QApplication::translate("MainFrameClass", "Full View", 0));
        actionMiniView->setText(QApplication::translate("MainFrameClass", "Mini View", 0));
        actionQuit->setText(QApplication::translate("MainFrameClass", "Quit...", 0));
        actionQuit->setShortcut(QApplication::translate("MainFrameClass", "Esc", 0));
        actionFullscreenMode->setText(QApplication::translate("MainFrameClass", "FullscreenMode", 0));
        actionFullscreenMode->setShortcut(QApplication::translate("MainFrameClass", "F11", 0));
        labelSectionTitle->setText(QApplication::translate("MainFrameClass", "Your controls", 0));
#ifndef QT_NO_TOOLTIP
        localControlsCollapseButton->setToolTip(QApplication::translate("MainFrameClass", "collapse/expand your controls ...", 0));
#endif // QT_NO_TOOLTIP
        localControlsCollapseButton->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainFrameClass", "Rooms to play", 0));
        menuPreferences->setTitle(QApplication::translate("MainFrameClass", "Preferences", 0));
        menuNinjam->setTitle(QApplication::translate("MainFrameClass", "Ninjam", 0));
        menuHelp->setTitle(QApplication::translate("MainFrameClass", "Help", 0));
        menuViewMode->setTitle(QApplication::translate("MainFrameClass", "View Mode", 0));
        Q_UNUSED(MainFrameClass);
    } // retranslateUi

};

namespace Ui {
    class MainFrameClass: public Ui_MainFrameClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
