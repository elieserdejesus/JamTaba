/********************************************************************************
** Form generated from reading UI file 'BaseTrackView.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BASETRACKVIEW_H
#define UI_BASETRACKVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDial>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "peakmeter.h"

QT_BEGIN_NAMESPACE

class Ui_BaseTrackView
{
public:
    QVBoxLayout *mainLayout;
    QWidget *mainWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *leftWidget;
    QGridLayout *gridLayout_2;
    QSlider *levelSlider;
    QSpacerItem *panSpacer;
    QWidget *panPanel;
    QGridLayout *gridLayout;
    QLabel *labelPanL;
    QLabel *labelPanR;
    QDial *panSlider;
    QWidget *rightWidget;
    QVBoxLayout *verticalLayout_2;
    QWidget *metersWidget;
    QHBoxLayout *horizontalLayout_2;
    PeakMeter *peakMeterLeft;
    PeakMeter *peakMeterRight;
    QLabel *peaksDbLabel;
    QPushButton *muteButton;
    QPushButton *soloButton;
    QGroupBox *boostPanel;
    QVBoxLayout *verticalLayout;
    QPushButton *buttonBoostPlus12;
    QPushButton *buttonBoostZero;
    QPushButton *buttonBoostMinus12;

    void setupUi(QWidget *BaseTrackView)
    {
        if (BaseTrackView->objectName().isEmpty())
            BaseTrackView->setObjectName(QStringLiteral("BaseTrackView"));
        BaseTrackView->resize(208, 403);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(BaseTrackView->sizePolicy().hasHeightForWidth());
        BaseTrackView->setSizePolicy(sizePolicy);
        BaseTrackView->setStyleSheet(QStringLiteral(""));
        mainLayout = new QVBoxLayout(BaseTrackView);
        mainLayout->setSpacing(3);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        mainLayout->setContentsMargins(3, 3, 3, 3);
        mainWidget = new QWidget(BaseTrackView);
        mainWidget->setObjectName(QStringLiteral("mainWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(mainWidget->sizePolicy().hasHeightForWidth());
        mainWidget->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(mainWidget);
        horizontalLayout->setSpacing(3);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 12, 0, 0);
        leftWidget = new QWidget(mainWidget);
        leftWidget->setObjectName(QStringLiteral("leftWidget"));
        gridLayout_2 = new QGridLayout(leftWidget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(0);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        levelSlider = new QSlider(leftWidget);
        levelSlider->setObjectName(QStringLiteral("levelSlider"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(levelSlider->sizePolicy().hasHeightForWidth());
        levelSlider->setSizePolicy(sizePolicy2);
        levelSlider->setMaximum(120);
        levelSlider->setValue(100);
        levelSlider->setOrientation(Qt::Vertical);
        levelSlider->setTickPosition(QSlider::NoTicks);
        levelSlider->setTickInterval(0);

        gridLayout_2->addWidget(levelSlider, 2, 1, 1, 1, Qt::AlignHCenter);

        panSpacer = new QSpacerItem(20, 12, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_2->addItem(panSpacer, 1, 0, 1, 2);

        panPanel = new QWidget(leftWidget);
        panPanel->setObjectName(QStringLiteral("panPanel"));
        panPanel->setStyleSheet(QStringLiteral(""));
        gridLayout = new QGridLayout(panPanel);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        labelPanL = new QLabel(panPanel);
        labelPanL->setObjectName(QStringLiteral("labelPanL"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(labelPanL->sizePolicy().hasHeightForWidth());
        labelPanL->setSizePolicy(sizePolicy3);
        labelPanL->setMinimumSize(QSize(26, 0));
        labelPanL->setMaximumSize(QSize(32, 16777215));
        labelPanL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(labelPanL, 1, 1, 1, 1, Qt::AlignRight);

        labelPanR = new QLabel(panPanel);
        labelPanR->setObjectName(QStringLiteral("labelPanR"));
        sizePolicy3.setHeightForWidth(labelPanR->sizePolicy().hasHeightForWidth());
        labelPanR->setSizePolicy(sizePolicy3);
        labelPanR->setMinimumSize(QSize(26, 0));
        labelPanR->setMaximumSize(QSize(32, 16777215));
        labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(labelPanR, 1, 2, 1, 1, Qt::AlignLeft);

        panSlider = new QDial(panPanel);
        panSlider->setObjectName(QStringLiteral("panSlider"));
        panSlider->setMinimum(-4);
        panSlider->setMaximum(4);
        panSlider->setPageStep(1);
        panSlider->setTracking(false);
        panSlider->setWrapping(false);
        panSlider->setNotchesVisible(false);

        gridLayout->addWidget(panSlider, 0, 1, 2, 2, Qt::AlignHCenter);


        gridLayout_2->addWidget(panPanel, 0, 0, 1, 2);


        horizontalLayout->addWidget(leftWidget);

        rightWidget = new QWidget(mainWidget);
        rightWidget->setObjectName(QStringLiteral("rightWidget"));
        QSizePolicy sizePolicy4(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(rightWidget->sizePolicy().hasHeightForWidth());
        rightWidget->setSizePolicy(sizePolicy4);
        rightWidget->setStyleSheet(QStringLiteral(""));
        verticalLayout_2 = new QVBoxLayout(rightWidget);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        metersWidget = new QWidget(rightWidget);
        metersWidget->setObjectName(QStringLiteral("metersWidget"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(1);
        sizePolicy5.setHeightForWidth(metersWidget->sizePolicy().hasHeightForWidth());
        metersWidget->setSizePolicy(sizePolicy5);
        horizontalLayout_2 = new QHBoxLayout(metersWidget);
        horizontalLayout_2->setSpacing(1);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        peakMeterLeft = new PeakMeter(metersWidget);
        peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(peakMeterLeft->sizePolicy().hasHeightForWidth());
        peakMeterLeft->setSizePolicy(sizePolicy6);

        horizontalLayout_2->addWidget(peakMeterLeft, 0, Qt::AlignLeft);

        peakMeterRight = new PeakMeter(metersWidget);
        peakMeterRight->setObjectName(QStringLiteral("peakMeterRight"));
        sizePolicy6.setHeightForWidth(peakMeterRight->sizePolicy().hasHeightForWidth());
        peakMeterRight->setSizePolicy(sizePolicy6);

        horizontalLayout_2->addWidget(peakMeterRight, 0, Qt::AlignRight);


        verticalLayout_2->addWidget(metersWidget);

        peaksDbLabel = new QLabel(rightWidget);
        peaksDbLabel->setObjectName(QStringLiteral("peaksDbLabel"));
        QSizePolicy sizePolicy7(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(peaksDbLabel->sizePolicy().hasHeightForWidth());
        peaksDbLabel->setSizePolicy(sizePolicy7);

        verticalLayout_2->addWidget(peaksDbLabel);

        muteButton = new QPushButton(rightWidget);
        muteButton->setObjectName(QStringLiteral("muteButton"));
        muteButton->setEnabled(true);
        muteButton->setLayoutDirection(Qt::LeftToRight);
        muteButton->setIconSize(QSize(0, 0));
        muteButton->setCheckable(true);

        verticalLayout_2->addWidget(muteButton);

        soloButton = new QPushButton(rightWidget);
        soloButton->setObjectName(QStringLiteral("soloButton"));
        soloButton->setLayoutDirection(Qt::LeftToRight);
        soloButton->setCheckable(true);

        verticalLayout_2->addWidget(soloButton);

        boostPanel = new QGroupBox(rightWidget);
        boostPanel->setObjectName(QStringLiteral("boostPanel"));
        boostPanel->setStyleSheet(QStringLiteral(""));
        verticalLayout = new QVBoxLayout(boostPanel);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        buttonBoostPlus12 = new QPushButton(boostPanel);
        buttonBoostPlus12->setObjectName(QStringLiteral("buttonBoostPlus12"));
        buttonBoostPlus12->setCheckable(true);

        verticalLayout->addWidget(buttonBoostPlus12);

        buttonBoostZero = new QPushButton(boostPanel);
        buttonBoostZero->setObjectName(QStringLiteral("buttonBoostZero"));
        buttonBoostZero->setCheckable(true);

        verticalLayout->addWidget(buttonBoostZero);

        buttonBoostMinus12 = new QPushButton(boostPanel);
        buttonBoostMinus12->setObjectName(QStringLiteral("buttonBoostMinus12"));
        buttonBoostMinus12->setCheckable(true);

        verticalLayout->addWidget(buttonBoostMinus12);


        verticalLayout_2->addWidget(boostPanel);


        horizontalLayout->addWidget(rightWidget);


        mainLayout->addWidget(mainWidget);


        retranslateUi(BaseTrackView);

        QMetaObject::connectSlotsByName(BaseTrackView);
    } // setupUi

    void retranslateUi(QWidget *BaseTrackView)
    {
        BaseTrackView->setWindowTitle(QApplication::translate("BaseTrackView", "Form", 0));
#ifndef QT_NO_ACCESSIBILITY
        levelSlider->setAccessibleDescription(QApplication::translate("BaseTrackView", "This is the volume slider, Moove up to increase the volume or down to decrease it ", 0));
#endif // QT_NO_ACCESSIBILITY
        labelPanL->setText(QApplication::translate("BaseTrackView", "L", 0));
        labelPanR->setText(QApplication::translate("BaseTrackView", "R", 0));
#ifndef QT_NO_ACCESSIBILITY
        panSlider->setAccessibleDescription(QApplication::translate("BaseTrackView", "This is the Pan knob", 0));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        peakMeterLeft->setAccessibleDescription(QApplication::translate("BaseTrackView", "This is the peak meter left ", 0));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        peakMeterRight->setAccessibleDescription(QApplication::translate("BaseTrackView", "This is the peak meter right ", 0));
#endif // QT_NO_ACCESSIBILITY
        peaksDbLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        muteButton->setToolTip(QApplication::translate("BaseTrackView", "mute", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        muteButton->setAccessibleDescription(QApplication::translate("BaseTrackView", "This is the mute button", 0));
#endif // QT_NO_ACCESSIBILITY
        muteButton->setText(QApplication::translate("BaseTrackView", "M", 0));
#ifndef QT_NO_TOOLTIP
        soloButton->setToolTip(QApplication::translate("BaseTrackView", "solo", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        soloButton->setAccessibleDescription(QApplication::translate("BaseTrackView", "this is the solo mode button", 0));
#endif // QT_NO_ACCESSIBILITY
        soloButton->setText(QApplication::translate("BaseTrackView", "S", 0));
        boostPanel->setTitle(QString());
#ifndef QT_NO_TOOLTIP
        buttonBoostPlus12->setToolTip(QApplication::translate("BaseTrackView", "+12 db boost", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        buttonBoostPlus12->setAccessibleDescription(QApplication::translate("BaseTrackView", "this is the +12 decibel button", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonBoostPlus12->setText(QApplication::translate("BaseTrackView", "+12", 0));
#ifndef QT_NO_TOOLTIP
        buttonBoostZero->setToolTip(QApplication::translate("BaseTrackView", "no boost", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        buttonBoostZero->setAccessibleDescription(QApplication::translate("BaseTrackView", "This is the zero decibel button", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonBoostZero->setText(QApplication::translate("BaseTrackView", "0", 0));
#ifndef QT_NO_TOOLTIP
        buttonBoostMinus12->setToolTip(QApplication::translate("BaseTrackView", "-12 db boost", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        buttonBoostMinus12->setAccessibleDescription(QApplication::translate("BaseTrackView", "This is -12 decibels button", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonBoostMinus12->setText(QApplication::translate("BaseTrackView", "-12", 0));
    } // retranslateUi

};

namespace Ui {
    class BaseTrackView: public Ui_BaseTrackView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BASETRACKVIEW_H
