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
#include <QtWidgets/QGridLayout>
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
    QWidget *topPanel;
    QGridLayout *topPanelLayout;
    QLabel *labelPanR;
    QLabel *labelPanL;
    QSlider *panSlider;
    QWidget *soloMutePane;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *soloButton;
    QPushButton *muteButton;
    QSpacerItem *panSpacer;
    QWidget *faderPanel;
    QHBoxLayout *horizontalLayout;
    QSlider *levelSlider;
    QWidget *peaksPanel;
    QHBoxLayout *horizontalLayout_3;
    PeakMeter *peakMeterLeft;
    PeakMeter *peakMeterRight;

    void setupUi(QWidget *BaseTrackView)
    {
        if (BaseTrackView->objectName().isEmpty())
            BaseTrackView->setObjectName(QStringLiteral("BaseTrackView"));
        BaseTrackView->resize(120, 574);
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
        labelPanR = new QLabel(topPanel);
        labelPanR->setObjectName(QStringLiteral("labelPanR"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelPanR->sizePolicy().hasHeightForWidth());
        labelPanR->setSizePolicy(sizePolicy2);
        labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        topPanelLayout->addWidget(labelPanR, 1, 2, 1, 1);

        labelPanL = new QLabel(topPanel);
        labelPanL->setObjectName(QStringLiteral("labelPanL"));
        sizePolicy2.setHeightForWidth(labelPanL->sizePolicy().hasHeightForWidth());
        labelPanL->setSizePolicy(sizePolicy2);
        labelPanL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        topPanelLayout->addWidget(labelPanL, 1, 0, 1, 1);

        panSlider = new QSlider(topPanel);
        panSlider->setObjectName(QStringLiteral("panSlider"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(panSlider->sizePolicy().hasHeightForWidth());
        panSlider->setSizePolicy(sizePolicy3);
        panSlider->setMinimum(-4);
        panSlider->setMaximum(4);
        panSlider->setSingleStep(1);
        panSlider->setPageStep(1);
        panSlider->setValue(0);
        panSlider->setSliderPosition(0);
        panSlider->setTracking(true);
        panSlider->setOrientation(Qt::Horizontal);
        panSlider->setInvertedAppearance(false);
        panSlider->setTickPosition(QSlider::TicksBelow);

        topPanelLayout->addWidget(panSlider, 1, 1, 1, 1);

        soloMutePane = new QWidget(topPanel);
        soloMutePane->setObjectName(QStringLiteral("soloMutePane"));
        sizePolicy1.setHeightForWidth(soloMutePane->sizePolicy().hasHeightForWidth());
        soloMutePane->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(soloMutePane);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        soloButton = new QPushButton(soloMutePane);
        soloButton->setObjectName(QStringLiteral("soloButton"));
        QSizePolicy sizePolicy4(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(soloButton->sizePolicy().hasHeightForWidth());
        soloButton->setSizePolicy(sizePolicy4);
        soloButton->setMinimumSize(QSize(20, 0));
        soloButton->setLayoutDirection(Qt::LeftToRight);
        soloButton->setCheckable(true);

        horizontalLayout_2->addWidget(soloButton, 0, Qt::AlignLeft);

        muteButton = new QPushButton(soloMutePane);
        muteButton->setObjectName(QStringLiteral("muteButton"));
        muteButton->setEnabled(true);
        sizePolicy4.setHeightForWidth(muteButton->sizePolicy().hasHeightForWidth());
        muteButton->setSizePolicy(sizePolicy4);
        muteButton->setMinimumSize(QSize(20, 0));
        muteButton->setLayoutDirection(Qt::LeftToRight);
        muteButton->setIconSize(QSize(0, 0));
        muteButton->setCheckable(true);

        horizontalLayout_2->addWidget(muteButton, 0, Qt::AlignRight);


        topPanelLayout->addWidget(soloMutePane, 0, 0, 1, 3);


        mainLayout->addWidget(topPanel);

        panSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        mainLayout->addItem(panSpacer);

        faderPanel = new QWidget(BaseTrackView);
        faderPanel->setObjectName(QStringLiteral("faderPanel"));
        QSizePolicy sizePolicy5(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(1);
        sizePolicy5.setHeightForWidth(faderPanel->sizePolicy().hasHeightForWidth());
        faderPanel->setSizePolicy(sizePolicy5);
        horizontalLayout = new QHBoxLayout(faderPanel);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        levelSlider = new QSlider(faderPanel);
        levelSlider->setObjectName(QStringLiteral("levelSlider"));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(levelSlider->sizePolicy().hasHeightForWidth());
        levelSlider->setSizePolicy(sizePolicy6);
        levelSlider->setMaximum(120);
        levelSlider->setValue(100);
        levelSlider->setOrientation(Qt::Vertical);
        levelSlider->setTickPosition(QSlider::TicksBelow);
        levelSlider->setTickInterval(0);

        horizontalLayout->addWidget(levelSlider);

        peaksPanel = new QWidget(faderPanel);
        peaksPanel->setObjectName(QStringLiteral("peaksPanel"));
        QSizePolicy sizePolicy7(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(peaksPanel->sizePolicy().hasHeightForWidth());
        peaksPanel->setSizePolicy(sizePolicy7);
        horizontalLayout_3 = new QHBoxLayout(peaksPanel);
        horizontalLayout_3->setSpacing(2);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        peakMeterLeft = new PeakMeter(peaksPanel);
        peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
        peakMeterLeft->setStyleSheet(QStringLiteral(""));

        horizontalLayout_3->addWidget(peakMeterLeft);

        peakMeterRight = new PeakMeter(peaksPanel);
        peakMeterRight->setObjectName(QStringLiteral("peakMeterRight"));

        horizontalLayout_3->addWidget(peakMeterRight);


        horizontalLayout->addWidget(peaksPanel, 0, Qt::AlignRight);


        mainLayout->addWidget(faderPanel);


        retranslateUi(BaseTrackView);

        QMetaObject::connectSlotsByName(BaseTrackView);
    } // setupUi

    void retranslateUi(QWidget *BaseTrackView)
    {
        BaseTrackView->setWindowTitle(QApplication::translate("BaseTrackView", "Form", 0));
        labelPanR->setText(QApplication::translate("BaseTrackView", "R", 0));
        labelPanL->setText(QApplication::translate("BaseTrackView", "L", 0));
        soloButton->setText(QApplication::translate("BaseTrackView", "SOLO", 0));
        muteButton->setText(QApplication::translate("BaseTrackView", "MUTE", 0));
    } // retranslateUi

};

namespace Ui {
    class BaseTrackView: public Ui_BaseTrackView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BASETRACKVIEW_H
