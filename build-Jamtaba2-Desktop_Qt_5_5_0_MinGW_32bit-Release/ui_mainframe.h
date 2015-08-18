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
#include <QtWidgets/QGridLayout>
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
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *leftPanel;
    QGridLayout *gridLayout;
    QLabel *labelSectionTitle;
    QPushButton *toolButton;
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
        gridLayout = new QGridLayout(leftPanel);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(6, -1, 6, 6);
        labelSectionTitle = new QLabel(leftPanel);
        labelSectionTitle->setObjectName(QStringLiteral("labelSectionTitle"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelSectionTitle->sizePolicy().hasHeightForWidth());
        labelSectionTitle->setSizePolicy(sizePolicy);

        gridLayout->addWidget(labelSectionTitle, 0, 0, 1, 1);

        toolButton = new QPushButton(leftPanel);
        toolButton->setObjectName(QStringLiteral("toolButton"));

        gridLayout->addWidget(toolButton, 0, 1, 1, 1);

        line = new QFrame(leftPanel);
        line->setObjectName(QStringLiteral("line"));
        line->setAutoFillBackground(false);
        line->setFrameShadow(QFrame::Sunken);
        line->setFrameShape(QFrame::HLine);

        gridLayout->addWidget(line, 1, 0, 1, 2);

        localTracksWidget = new QWidget(leftPanel);
        localTracksWidget->setObjectName(QStringLiteral("localTracksWidget"));
        localTracksLayout = new QHBoxLayout(localTracksWidget);
        localTracksLayout->setSpacing(3);
        localTracksLayout->setContentsMargins(11, 11, 11, 11);
        localTracksLayout->setObjectName(QStringLiteral("localTracksLayout"));
        localTracksLayout->setContentsMargins(0, 0, 0, 0);

        gridLayout->addWidget(localTracksWidget, 2, 0, 1, 2);

        xmitButton = new QPushButton(leftPanel);
        xmitButton->setObjectName(QStringLiteral("xmitButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(xmitButton->sizePolicy().hasHeightForWidth());
        xmitButton->setSizePolicy(sizePolicy1);
        xmitButton->setCheckable(true);

        gridLayout->addWidget(xmitButton, 3, 0, 1, 2);


        horizontalLayout->addWidget(leftPanel);

        contentPanel = new QWidget(centralWidget);
        contentPanel->setObjectName(QStringLiteral("contentPanel"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(2);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(contentPanel->sizePolicy().hasHeightForWidth());
        contentPanel->setSizePolicy(sizePolicy2);
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
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(allRoomsContent->sizePolicy().hasHeightForWidth());
        allRoomsContent->setSizePolicy(sizePolicy3);

        verticalLayout_6->addWidget(allRoomsContent);

        allRoomsScroll->setWidget(scrollAreaWidgetContents);

        verticalLayout_5->addWidget(allRoomsScroll);

        tabWidget->addTab(tab, QString());

        verticalLayout_3->addWidget(tabWidget);


        horizontalLayout->addWidget(contentPanel);

        chatArea = new QWidget(centralWidget);
        chatArea->setObjectName(QStringLiteral("chatArea"));
        QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(1);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(chatArea->sizePolicy().hasHeightForWidth());
        chatArea->setSizePolicy(sizePolicy4);
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
        QSizePolicy sizePolicy5(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(chatTabWidget->sizePolicy().hasHeightForWidth());
        chatTabWidget->setSizePolicy(sizePolicy5);

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
        labelSectionTitle->setText(QApplication::translate("MainFrameClass", "Your controls", 0));
#ifndef QT_NO_TOOLTIP
        toolButton->setToolTip(QApplication::translate("MainFrameClass", "add or remove channels ...", 0));
#endif // QT_NO_TOOLTIP
        toolButton->setText(QString());
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
