/********************************************************************************
** Form generated from reading UI file 'BaseTrackView.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BASETRACKVIEW_H
#define UI_BASETRACKVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "peakmeter.h"

QT_BEGIN_NAMESPACE

class Ui_TrackView
{
public:
    QVBoxLayout *mainLayout;
    QWidget *namesPanel;
    QVBoxLayout *topLayout;
    QLabel *trackName;
    QLineEdit *channelName;
    QFrame *topSeparator;
    QWidget *topPanel;
    QGridLayout *topPanelLayout;
    QWidget *soloMutePane;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *soloButton;
    QPushButton *muteButton;
    QLabel *labelPanR;
    QLabel *labelPanL;
    QSlider *panSlider;
    QFrame *panSectionSeparator;
    QWidget *faderPanel;
    QHBoxLayout *horizontalLayout;
    QSlider *levelSlider;
    QWidget *peaksPanel;
    QHBoxLayout *horizontalLayout_3;
    PeakMeter *peakMeterLeft;
    PeakMeter *peakMeterRight;

    void setupUi(QWidget *TrackView)
    {
        if (TrackView->objectName().isEmpty())
            TrackView->setObjectName(QStringLiteral("TrackView"));
        TrackView->resize(174, 574);
        mainLayout = new QVBoxLayout(TrackView);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        namesPanel = new QWidget(TrackView);
        namesPanel->setObjectName(QStringLiteral("namesPanel"));
        topLayout = new QVBoxLayout(namesPanel);
        topLayout->setObjectName(QStringLiteral("topLayout"));
        topLayout->setContentsMargins(0, 0, 0, 0);
        trackName = new QLabel(namesPanel);
        trackName->setObjectName(QStringLiteral("trackName"));
        trackName->setAlignment(Qt::AlignCenter);

        topLayout->addWidget(trackName);

        channelName = new QLineEdit(namesPanel);
        channelName->setObjectName(QStringLiteral("channelName"));
        channelName->setFrame(false);
        channelName->setAlignment(Qt::AlignCenter);
        channelName->setClearButtonEnabled(false);

        topLayout->addWidget(channelName);


        mainLayout->addWidget(namesPanel);

        topSeparator = new QFrame(TrackView);
        topSeparator->setObjectName(QStringLiteral("topSeparator"));
        topSeparator->setFrameShape(QFrame::HLine);
        topSeparator->setFrameShadow(QFrame::Sunken);

        mainLayout->addWidget(topSeparator);

        topPanel = new QWidget(TrackView);
        topPanel->setObjectName(QStringLiteral("topPanel"));
        topPanelLayout = new QGridLayout(topPanel);
        topPanelLayout->setObjectName(QStringLiteral("topPanelLayout"));
        topPanelLayout->setVerticalSpacing(12);
        topPanelLayout->setContentsMargins(0, -1, 0, -1);
        soloMutePane = new QWidget(topPanel);
        soloMutePane->setObjectName(QStringLiteral("soloMutePane"));
        horizontalLayout_2 = new QHBoxLayout(soloMutePane);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        soloButton = new QPushButton(soloMutePane);
        soloButton->setObjectName(QStringLiteral("soloButton"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(soloButton->sizePolicy().hasHeightForWidth());
        soloButton->setSizePolicy(sizePolicy);
        soloButton->setLayoutDirection(Qt::LeftToRight);
        soloButton->setCheckable(true);

        horizontalLayout_2->addWidget(soloButton, 0, Qt::AlignLeft);

        muteButton = new QPushButton(soloMutePane);
        muteButton->setObjectName(QStringLiteral("muteButton"));
        muteButton->setEnabled(true);
        sizePolicy.setHeightForWidth(muteButton->sizePolicy().hasHeightForWidth());
        muteButton->setSizePolicy(sizePolicy);
        muteButton->setLayoutDirection(Qt::LeftToRight);
        muteButton->setCheckable(true);

        horizontalLayout_2->addWidget(muteButton, 0, Qt::AlignRight);


        topPanelLayout->addWidget(soloMutePane, 1, 0, 1, 3);

        labelPanR = new QLabel(topPanel);
        labelPanR->setObjectName(QStringLiteral("labelPanR"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelPanR->sizePolicy().hasHeightForWidth());
        labelPanR->setSizePolicy(sizePolicy1);
        labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        topPanelLayout->addWidget(labelPanR, 0, 2, 1, 1);

        labelPanL = new QLabel(topPanel);
        labelPanL->setObjectName(QStringLiteral("labelPanL"));
        sizePolicy1.setHeightForWidth(labelPanL->sizePolicy().hasHeightForWidth());
        labelPanL->setSizePolicy(sizePolicy1);
        labelPanL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        topPanelLayout->addWidget(labelPanL, 0, 0, 1, 1);

        panSlider = new QSlider(topPanel);
        panSlider->setObjectName(QStringLiteral("panSlider"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(panSlider->sizePolicy().hasHeightForWidth());
        panSlider->setSizePolicy(sizePolicy2);
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

        topPanelLayout->addWidget(panSlider, 0, 1, 1, 1);


        mainLayout->addWidget(topPanel);

        panSectionSeparator = new QFrame(TrackView);
        panSectionSeparator->setObjectName(QStringLiteral("panSectionSeparator"));
        panSectionSeparator->setFrameShape(QFrame::HLine);
        panSectionSeparator->setFrameShadow(QFrame::Sunken);

        mainLayout->addWidget(panSectionSeparator);

        faderPanel = new QWidget(TrackView);
        faderPanel->setObjectName(QStringLiteral("faderPanel"));
        horizontalLayout = new QHBoxLayout(faderPanel);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        levelSlider = new QSlider(faderPanel);
        levelSlider->setObjectName(QStringLiteral("levelSlider"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(3);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(levelSlider->sizePolicy().hasHeightForWidth());
        levelSlider->setSizePolicy(sizePolicy3);
        levelSlider->setMaximum(120);
        levelSlider->setValue(100);
        levelSlider->setOrientation(Qt::Vertical);
        levelSlider->setTickPosition(QSlider::TicksBelow);
        levelSlider->setTickInterval(0);

        horizontalLayout->addWidget(levelSlider);

        peaksPanel = new QWidget(faderPanel);
        peaksPanel->setObjectName(QStringLiteral("peaksPanel"));
        horizontalLayout_3 = new QHBoxLayout(peaksPanel);
        horizontalLayout_3->setSpacing(3);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        peakMeterLeft = new PeakMeter(peaksPanel);
        peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
        peakMeterLeft->setStyleSheet(QStringLiteral(""));

        horizontalLayout_3->addWidget(peakMeterLeft);

        peakMeterRight = new PeakMeter(peaksPanel);
        peakMeterRight->setObjectName(QStringLiteral("peakMeterRight"));

        horizontalLayout_3->addWidget(peakMeterRight);


        horizontalLayout->addWidget(peaksPanel);


        mainLayout->addWidget(faderPanel);


        retranslateUi(TrackView);

        QMetaObject::connectSlotsByName(TrackView);
    } // setupUi

    void retranslateUi(QWidget *TrackView)
    {
        TrackView->setWindowTitle(QApplication::translate("TrackView", "Form", 0));
        trackName->setText(QApplication::translate("TrackView", "TextLabel", 0));
        channelName->setText(QApplication::translate("TrackView", "channel name", 0));
        soloButton->setText(QApplication::translate("TrackView", "SOLO", 0));
        muteButton->setText(QApplication::translate("TrackView", "MUTE", 0));
        labelPanR->setText(QApplication::translate("TrackView", "R", 0));
        labelPanL->setText(QApplication::translate("TrackView", "L", 0));
    } // retranslateUi

};

namespace Ui {
    class TrackView: public Ui_TrackView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BASETRACKVIEW_H
