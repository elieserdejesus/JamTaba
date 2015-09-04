/********************************************************************************
** Form generated from reading UI file 'mainframe.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainFrameClass
{
public:
    QAction *actionTest;
    QAction *actionAudioPreferences;
    QAction *actionMidiPreferences;
    QAction *actionVstPreferences;
    QAction *actionRecording;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *leftPanel;
    QVBoxLayout *verticalLayout_2;
    QWidget *topWidget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelSectionTitle;
    QPushButton *localControlsCollapseButton;
    QFrame *line;
    QWidget *localTracksWidget;
    QHBoxLayout *localTracksLayout;
    QPushButton *xmitButton;
    QWidget *contentPanel;
    QVBoxLayout *verticalLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_5;
    QScrollArea *allRoomsScroll;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_6;
    QWidget *allRoomsContent;
    QWidget *chatArea;
    QVBoxLayout *verticalLayout;
    QLabel *chatLabel;
    QFrame *chatDivider;
    QTabWidget *chatTabWidget;
    QMenuBar *menuBar;
    QMenu *menuPreferences;

    void setupUi(QMainWindow *MainFrameClass)
    {
        if (MainFrameClass->objectName().isEmpty())
            MainFrameClass->setObjectName(QStringLiteral("MainFrameClass"));
        MainFrameClass->resize(1160, 750);
        MainFrameClass->setMinimumSize(QSize(1160, 750));
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
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(leftPanel->sizePolicy().hasHeightForWidth());
        leftPanel->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(leftPanel);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout_2->setContentsMargins(6, -1, 6, 6);
        topWidget = new QWidget(leftPanel);
        topWidget->setObjectName(QStringLiteral("topWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
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

        line = new QFrame(leftPanel);
        line->setObjectName(QStringLiteral("line"));
        line->setAutoFillBackground(false);
        line->setFrameShadow(QFrame::Sunken);
        line->setFrameShape(QFrame::HLine);

        verticalLayout_2->addWidget(line);

        localTracksWidget = new QWidget(leftPanel);
        localTracksWidget->setObjectName(QStringLiteral("localTracksWidget"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(localTracksWidget->sizePolicy().hasHeightForWidth());
        localTracksWidget->setSizePolicy(sizePolicy3);
        localTracksLayout = new QHBoxLayout(localTracksWidget);
        localTracksLayout->setSpacing(3);
        localTracksLayout->setContentsMargins(11, 11, 11, 11);
        localTracksLayout->setObjectName(QStringLiteral("localTracksLayout"));
        localTracksLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        localTracksLayout->setContentsMargins(0, 0, 0, 0);

        verticalLayout_2->addWidget(localTracksWidget);

        xmitButton = new QPushButton(leftPanel);
        xmitButton->setObjectName(QStringLiteral("xmitButton"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(xmitButton->sizePolicy().hasHeightForWidth());
        xmitButton->setSizePolicy(sizePolicy4);
        xmitButton->setCheckable(true);

        verticalLayout_2->addWidget(xmitButton);


        horizontalLayout->addWidget(leftPanel);

        contentPanel = new QWidget(centralWidget);
        contentPanel->setObjectName(QStringLiteral("contentPanel"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(2);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(contentPanel->sizePolicy().hasHeightForWidth());
        contentPanel->setSizePolicy(sizePolicy5);
        verticalLayout_3 = new QVBoxLayout(contentPanel);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
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
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 694, 674));
        verticalLayout_6 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_6->setSpacing(50);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        allRoomsContent = new QWidget(scrollAreaWidgetContents);
        allRoomsContent->setObjectName(QStringLiteral("allRoomsContent"));
        QSizePolicy sizePolicy6(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(allRoomsContent->sizePolicy().hasHeightForWidth());
        allRoomsContent->setSizePolicy(sizePolicy6);

        verticalLayout_6->addWidget(allRoomsContent);

        allRoomsScroll->setWidget(scrollAreaWidgetContents);

        verticalLayout_5->addWidget(allRoomsScroll);

        tabWidget->addTab(tab, QString());

        verticalLayout_3->addWidget(tabWidget);


        horizontalLayout->addWidget(contentPanel);

        chatArea = new QWidget(centralWidget);
        chatArea->setObjectName(QStringLiteral("chatArea"));
        QSizePolicy sizePolicy7(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy7.setHorizontalStretch(1);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(chatArea->sizePolicy().hasHeightForWidth());
        chatArea->setSizePolicy(sizePolicy7);
        chatArea->setMinimumSize(QSize(280, 0));
        chatArea->setMaximumSize(QSize(280, 16777215));
        verticalLayout = new QVBoxLayout(chatArea);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(6, 6, 6, 6);
        chatLabel = new QLabel(chatArea);
        chatLabel->setObjectName(QStringLiteral("chatLabel"));

        verticalLayout->addWidget(chatLabel);

        chatDivider = new QFrame(chatArea);
        chatDivider->setObjectName(QStringLiteral("chatDivider"));
        chatDivider->setFrameShape(QFrame::HLine);
        chatDivider->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(chatDivider);

        chatTabWidget = new QTabWidget(chatArea);
        chatTabWidget->setObjectName(QStringLiteral("chatTabWidget"));
        QSizePolicy sizePolicy8(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(chatTabWidget->sizePolicy().hasHeightForWidth());
        chatTabWidget->setSizePolicy(sizePolicy8);

        verticalLayout->addWidget(chatTabWidget);


        horizontalLayout->addWidget(chatArea);

        MainFrameClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainFrameClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1160, 21));
        menuPreferences = new QMenu(menuBar);
        menuPreferences->setObjectName(QStringLiteral("menuPreferences"));
        MainFrameClass->setMenuBar(menuBar);

        menuBar->addAction(menuPreferences->menuAction());
        menuPreferences->addAction(actionAudioPreferences);
        menuPreferences->addAction(actionMidiPreferences);
        menuPreferences->addAction(actionVstPreferences);
        menuPreferences->addAction(actionRecording);

        retranslateUi(MainFrameClass);

        tabWidget->setCurrentIndex(0);
        chatTabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainFrameClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainFrameClass)
    {
        actionTest->setText(QApplication::translate("MainFrameClass", "test", 0));
        actionAudioPreferences->setText(QApplication::translate("MainFrameClass", "Audio ...", 0));
        actionMidiPreferences->setText(QApplication::translate("MainFrameClass", "Midi ...", 0));
        actionVstPreferences->setText(QApplication::translate("MainFrameClass", "VST Plugins ...", 0));
        actionRecording->setText(QApplication::translate("MainFrameClass", "Recording ...", 0));
        labelSectionTitle->setText(QApplication::translate("MainFrameClass", "Your controls", 0));
#ifndef QT_NO_TOOLTIP
        localControlsCollapseButton->setToolTip(QApplication::translate("MainFrameClass", "collapse/expand your controls ...", 0));
#endif // QT_NO_TOOLTIP
        localControlsCollapseButton->setText(QString());
        xmitButton->setText(QApplication::translate("MainFrameClass", "Transmit", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainFrameClass", "Rooms to play", 0));
        chatLabel->setText(QApplication::translate("MainFrameClass", "Chat", 0));
        menuPreferences->setTitle(QApplication::translate("MainFrameClass", "Preferences", 0));
        Q_UNUSED(MainFrameClass);
    } // retranslateUi

};

namespace Ui {
    class MainFrameClass: public Ui_MainFrameClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFRAME_H
