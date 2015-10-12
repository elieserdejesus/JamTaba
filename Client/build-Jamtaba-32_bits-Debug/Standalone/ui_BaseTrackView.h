/********************************************************************************
** Form generated from reading UI file 'BaseTrackView.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
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
    QWidget *topPanel;
    QGridLayout *topPanelLayout;
    QDial *panSlider;
    QLabel *labelPanR;
    QLabel *labelPanL;
    QSpacerItem *panSpacer;
    QWidget *faderPanel;
    QGridLayout *gridLayout;
    PeakMeter *peakMeterLeft;
    PeakMeter *peakMeterRight;
    QSpacerItem *verticalSpacer;
    QLabel *peaksDbLabel;
    QPushButton *soloButton;
    QPushButton *muteButton;
    QSlider *levelSlider;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *BaseTrackView)
    {
        if (BaseTrackView->objectName().isEmpty())
            BaseTrackView->setObjectName(QStringLiteral("BaseTrackView"));
        BaseTrackView->resize(136, 403);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(BaseTrackView->sizePolicy().hasHeightForWidth());
        BaseTrackView->setSizePolicy(sizePolicy);
        mainLayout = new QVBoxLayout(BaseTrackView);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        mainLayout->setContentsMargins(6, 6, 6, 6);
        topPanel = new QWidget(BaseTrackView);
        topPanel->setObjectName(QStringLiteral("topPanel"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(topPanel->sizePolicy().hasHeightForWidth());
        topPanel->setSizePolicy(sizePolicy1);
        topPanelLayout = new QGridLayout(topPanel);
        topPanelLayout->setObjectName(QStringLiteral("topPanelLayout"));
        topPanelLayout->setHorizontalSpacing(6);
        topPanelLayout->setVerticalSpacing(12);
        topPanelLayout->setContentsMargins(0, -1, 0, -1);
        panSlider = new QDial(topPanel);
        panSlider->setObjectName(QStringLiteral("panSlider"));
        panSlider->setMinimum(-4);
        panSlider->setMaximum(4);
        panSlider->setPageStep(1);
        panSlider->setTracking(false);
        panSlider->setNotchesVisible(false);

        topPanelLayout->addWidget(panSlider, 0, 1, 1, 1);

        labelPanR = new QLabel(topPanel);
        labelPanR->setObjectName(QStringLiteral("labelPanR"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelPanR->sizePolicy().hasHeightForWidth());
        labelPanR->setSizePolicy(sizePolicy2);
        labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        topPanelLayout->addWidget(labelPanR, 0, 2, 1, 1, Qt::AlignLeft|Qt::AlignBottom);

        labelPanL = new QLabel(topPanel);
        labelPanL->setObjectName(QStringLiteral("labelPanL"));
        sizePolicy2.setHeightForWidth(labelPanL->sizePolicy().hasHeightForWidth());
        labelPanL->setSizePolicy(sizePolicy2);
        labelPanL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        topPanelLayout->addWidget(labelPanL, 0, 0, 1, 1, Qt::AlignRight|Qt::AlignBottom);


        mainLayout->addWidget(topPanel);

        panSpacer = new QSpacerItem(20, 12, QSizePolicy::Minimum, QSizePolicy::Fixed);

        mainLayout->addItem(panSpacer);

        faderPanel = new QWidget(BaseTrackView);
        faderPanel->setObjectName(QStringLiteral("faderPanel"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(faderPanel->sizePolicy().hasHeightForWidth());
        faderPanel->setSizePolicy(sizePolicy3);
        gridLayout = new QGridLayout(faderPanel);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setVerticalSpacing(3);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        peakMeterLeft = new PeakMeter(faderPanel);
        peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(1);
        sizePolicy4.setHeightForWidth(peakMeterLeft->sizePolicy().hasHeightForWidth());
        peakMeterLeft->setSizePolicy(sizePolicy4);
        peakMeterLeft->setMinimumSize(QSize(10, 0));

        gridLayout->addWidget(peakMeterLeft, 0, 2, 2, 1);

        peakMeterRight = new PeakMeter(faderPanel);
        peakMeterRight->setObjectName(QStringLiteral("peakMeterRight"));
        QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(1);
        sizePolicy5.setHeightForWidth(peakMeterRight->sizePolicy().hasHeightForWidth());
        peakMeterRight->setSizePolicy(sizePolicy5);
        peakMeterRight->setMinimumSize(QSize(10, 0));

        gridLayout->addWidget(peakMeterRight, 0, 3, 2, 1);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer, 3, 2, 1, 2);

        peaksDbLabel = new QLabel(faderPanel);
        peaksDbLabel->setObjectName(QStringLiteral("peaksDbLabel"));
        QSizePolicy sizePolicy6(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(peaksDbLabel->sizePolicy().hasHeightForWidth());
        peaksDbLabel->setSizePolicy(sizePolicy6);
        peaksDbLabel->setMaximumSize(QSize(20, 16777215));

        gridLayout->addWidget(peaksDbLabel, 2, 2, 1, 2);

        soloButton = new QPushButton(faderPanel);
        soloButton->setObjectName(QStringLiteral("soloButton"));
        QSizePolicy sizePolicy7(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(soloButton->sizePolicy().hasHeightForWidth());
        soloButton->setSizePolicy(sizePolicy7);
        soloButton->setMinimumSize(QSize(20, 0));
        soloButton->setLayoutDirection(Qt::LeftToRight);
        soloButton->setCheckable(true);

        gridLayout->addWidget(soloButton, 5, 2, 1, 2);

        muteButton = new QPushButton(faderPanel);
        muteButton->setObjectName(QStringLiteral("muteButton"));
        muteButton->setEnabled(true);
        sizePolicy7.setHeightForWidth(muteButton->sizePolicy().hasHeightForWidth());
        muteButton->setSizePolicy(sizePolicy7);
        muteButton->setMinimumSize(QSize(20, 0));
        muteButton->setLayoutDirection(Qt::LeftToRight);
        muteButton->setIconSize(QSize(0, 0));
        muteButton->setCheckable(true);

        gridLayout->addWidget(muteButton, 4, 2, 1, 2);

        levelSlider = new QSlider(faderPanel);
        levelSlider->setObjectName(QStringLiteral("levelSlider"));
        QSizePolicy sizePolicy8(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy8.setHorizontalStretch(1);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(levelSlider->sizePolicy().hasHeightForWidth());
        levelSlider->setSizePolicy(sizePolicy8);
        levelSlider->setMaximum(120);
        levelSlider->setValue(100);
        levelSlider->setOrientation(Qt::Vertical);
        levelSlider->setTickPosition(QSlider::NoTicks);
        levelSlider->setTickInterval(0);

        gridLayout->addWidget(levelSlider, 0, 1, 6, 1);

        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 0, 6, 1);


        mainLayout->addWidget(faderPanel);


        retranslateUi(BaseTrackView);

        QMetaObject::connectSlotsByName(BaseTrackView);
    } // setupUi

    void retranslateUi(QWidget *BaseTrackView)
    {
        BaseTrackView->setWindowTitle(QApplication::translate("BaseTrackView", "Form", 0));
        labelPanR->setText(QApplication::translate("BaseTrackView", "R", 0));
        labelPanL->setText(QApplication::translate("BaseTrackView", "L", 0));
        peaksDbLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        soloButton->setToolTip(QApplication::translate("BaseTrackView", "solo", 0));
#endif // QT_NO_TOOLTIP
        soloButton->setText(QApplication::translate("BaseTrackView", "S", 0));
#ifndef QT_NO_TOOLTIP
        muteButton->setToolTip(QApplication::translate("BaseTrackView", "mute", 0));
#endif // QT_NO_TOOLTIP
        muteButton->setText(QApplication::translate("BaseTrackView", "M", 0));
    } // retranslateUi

};

namespace Ui {
    class BaseTrackView: public Ui_BaseTrackView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BASETRACKVIEW_H
