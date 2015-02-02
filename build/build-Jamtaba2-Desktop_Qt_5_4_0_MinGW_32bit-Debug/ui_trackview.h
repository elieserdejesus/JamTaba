/********************************************************************************
** Form generated from reading UI file 'trackview.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKVIEW_H
#define UI_TRACKVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "PeakMeter.h"

QT_BEGIN_NAMESPACE

class Ui_TrackView
{
public:
    QGridLayout *gridLayout_3;
    QFrame *topSeparator;
    QWidget *widget_4;
    QGridLayout *gridLayout_2;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *solo;
    QPushButton *mute;
    QLabel *labelPanR;
    QLabel *labelPanL;
    QSlider *panSlider;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout;
    QSlider *verticalSlider;
    Gui::PeakMeter *widget;
    QGroupBox *transmit;
    QGridLayout *gridLayout;
    QLabel *gainLabel;
    Gui::PeakMeter *widget_3;
    QSlider *horizontalSlider;
    QFrame *panSectionSeparator;
    QWidget *widget_6;
    QVBoxLayout *verticalLayout;
    QLabel *trackName;
    QLineEdit *channelName;

    void setupUi(QWidget *TrackView)
    {
        if (TrackView->objectName().isEmpty())
            TrackView->setObjectName(QStringLiteral("TrackView"));
        TrackView->resize(174, 574);
        gridLayout_3 = new QGridLayout(TrackView);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setVerticalSpacing(20);
        topSeparator = new QFrame(TrackView);
        topSeparator->setObjectName(QStringLiteral("topSeparator"));
        topSeparator->setFrameShape(QFrame::HLine);
        topSeparator->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(topSeparator, 4, 0, 1, 1);

        widget_4 = new QWidget(TrackView);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        gridLayout_2 = new QGridLayout(widget_4);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(12);
        gridLayout_2->setContentsMargins(0, -1, 0, -1);
        widget_2 = new QWidget(widget_4);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        solo = new QPushButton(widget_2);
        solo->setObjectName(QStringLiteral("solo"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(solo->sizePolicy().hasHeightForWidth());
        solo->setSizePolicy(sizePolicy);
        solo->setLayoutDirection(Qt::LeftToRight);
        solo->setCheckable(true);

        horizontalLayout_2->addWidget(solo, 0, Qt::AlignLeft);

        mute = new QPushButton(widget_2);
        mute->setObjectName(QStringLiteral("mute"));
        mute->setEnabled(true);
        sizePolicy.setHeightForWidth(mute->sizePolicy().hasHeightForWidth());
        mute->setSizePolicy(sizePolicy);
        mute->setLayoutDirection(Qt::LeftToRight);
        mute->setCheckable(true);

        horizontalLayout_2->addWidget(mute, 0, Qt::AlignRight);


        gridLayout_2->addWidget(widget_2, 1, 0, 1, 3);

        labelPanR = new QLabel(widget_4);
        labelPanR->setObjectName(QStringLiteral("labelPanR"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelPanR->sizePolicy().hasHeightForWidth());
        labelPanR->setSizePolicy(sizePolicy1);
        labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(labelPanR, 0, 2, 1, 1);

        labelPanL = new QLabel(widget_4);
        labelPanL->setObjectName(QStringLiteral("labelPanL"));
        sizePolicy1.setHeightForWidth(labelPanL->sizePolicy().hasHeightForWidth());
        labelPanL->setSizePolicy(sizePolicy1);
        labelPanL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(labelPanL, 0, 0, 1, 1);

        panSlider = new QSlider(widget_4);
        panSlider->setObjectName(QStringLiteral("panSlider"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(panSlider->sizePolicy().hasHeightForWidth());
        panSlider->setSizePolicy(sizePolicy2);
        panSlider->setSingleStep(10);
        panSlider->setValue(50);
        panSlider->setOrientation(Qt::Horizontal);
        panSlider->setInvertedAppearance(false);

        gridLayout_2->addWidget(panSlider, 0, 1, 1, 1);


        gridLayout_3->addWidget(widget_4, 5, 0, 1, 1);

        widget_5 = new QWidget(TrackView);
        widget_5->setObjectName(QStringLiteral("widget_5"));
        horizontalLayout = new QHBoxLayout(widget_5);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalSlider = new QSlider(widget_5);
        verticalSlider->setObjectName(QStringLiteral("verticalSlider"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(3);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(verticalSlider->sizePolicy().hasHeightForWidth());
        verticalSlider->setSizePolicy(sizePolicy3);
        verticalSlider->setValue(90);
        verticalSlider->setOrientation(Qt::Vertical);
        verticalSlider->setTickPosition(QSlider::TicksBothSides);

        horizontalLayout->addWidget(verticalSlider);

        widget = new Gui::PeakMeter(widget_5);
        widget->setObjectName(QStringLiteral("widget"));
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        widget->setMinimumSize(QSize(20, 0));
        widget->setStyleSheet(QStringLiteral(""));

        horizontalLayout->addWidget(widget);


        gridLayout_3->addWidget(widget_5, 7, 0, 1, 1);

        transmit = new QGroupBox(TrackView);
        transmit->setObjectName(QStringLiteral("transmit"));
        transmit->setCheckable(true);
        gridLayout = new QGridLayout(transmit);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setVerticalSpacing(6);
        gridLayout->setContentsMargins(0, 12, 0, 10);
        gainLabel = new QLabel(transmit);
        gainLabel->setObjectName(QStringLiteral("gainLabel"));
        gainLabel->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(gainLabel, 0, 0, 1, 1);

        widget_3 = new Gui::PeakMeter(transmit);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        sizePolicy1.setHeightForWidth(widget_3->sizePolicy().hasHeightForWidth());
        widget_3->setSizePolicy(sizePolicy1);
        widget_3->setMinimumSize(QSize(20, 20));
        widget_3->setMaximumSize(QSize(20, 16777215));

        gridLayout->addWidget(widget_3, 0, 2, 2, 1, Qt::AlignRight);

        horizontalSlider = new QSlider(transmit);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSlider, 1, 0, 1, 1);


        gridLayout_3->addWidget(transmit, 8, 0, 1, 1);

        panSectionSeparator = new QFrame(TrackView);
        panSectionSeparator->setObjectName(QStringLiteral("panSectionSeparator"));
        panSectionSeparator->setFrameShape(QFrame::HLine);
        panSectionSeparator->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(panSectionSeparator, 6, 0, 1, 1);

        widget_6 = new QWidget(TrackView);
        widget_6->setObjectName(QStringLiteral("widget_6"));
        verticalLayout = new QVBoxLayout(widget_6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        trackName = new QLabel(widget_6);
        trackName->setObjectName(QStringLiteral("trackName"));
        trackName->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(trackName);

        channelName = new QLineEdit(widget_6);
        channelName->setObjectName(QStringLiteral("channelName"));
        channelName->setFrame(false);
        channelName->setAlignment(Qt::AlignCenter);
        channelName->setClearButtonEnabled(false);

        verticalLayout->addWidget(channelName);


        gridLayout_3->addWidget(widget_6, 0, 0, 1, 1);


        retranslateUi(TrackView);

        QMetaObject::connectSlotsByName(TrackView);
    } // setupUi

    void retranslateUi(QWidget *TrackView)
    {
        TrackView->setWindowTitle(QApplication::translate("TrackView", "Form", 0));
        solo->setText(QApplication::translate("TrackView", "SOLO", 0));
        mute->setText(QApplication::translate("TrackView", "MUTE", 0));
        labelPanR->setText(QApplication::translate("TrackView", "R", 0));
        labelPanL->setText(QApplication::translate("TrackView", "L", 0));
        transmit->setTitle(QApplication::translate("TrackView", "transmit", 0));
        gainLabel->setText(QApplication::translate("TrackView", "gain", 0));
        trackName->setText(QApplication::translate("TrackView", "TextLabel", 0));
        channelName->setText(QApplication::translate("TrackView", "channel name", 0));
    } // retranslateUi

};

namespace Ui {
    class TrackView: public Ui_TrackView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACKVIEW_H
