/********************************************************************************
** Form generated from reading UI file 'NinjamPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NINJAMPANEL_H
#define UI_NINJAMPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "IntervalProgressDisplay.h"
#include "peakmeter.h"

QT_BEGIN_NAMESPACE

class Ui_NinjamPanel
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QWidget *metronomeControls;
    QGridLayout *gridLayout_4;
    QPushButton *soloButton;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *muteButton;
    QWidget *panPanel;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelPanL;
    QSlider *panSlider;
    QLabel *labelPanR;
    QWidget *peaks;
    QHBoxLayout *horizontalLayout;
    PeakMeter *peakMeterLeft;
    PeakMeter *peakMeterRight;
    QSlider *levelSlider;
    QLabel *labelMetronomeIcon;
    IntervalProgressDisplay *intervalPanel;
    QWidget *panelCombos;
    QGridLayout *gridLayout_3;
    QLabel *labelBPI;
    QComboBox *comboBpi;
    QComboBox *comboBpm;
    QLabel *labelBPM;
    QComboBox *comboBeatsPerAccent;
    QLabel *labelAccent;
    QLabel *labelShape;
    QComboBox *comboShape;

    void setupUi(QWidget *NinjamPanel)
    {
        if (NinjamPanel->objectName().isEmpty())
            NinjamPanel->setObjectName(QStringLiteral("NinjamPanel"));
        NinjamPanel->resize(566, 165);
        NinjamPanel->setMaximumSize(QSize(16777215, 165));
        gridLayout = new QGridLayout(NinjamPanel);
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 12, 0, 12);
        horizontalSpacer = new QSpacerItem(10, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 3, 3, 1);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 5, 3, 1);

        widget = new QWidget(NinjamPanel);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        metronomeControls = new QWidget(widget);
        metronomeControls->setObjectName(QStringLiteral("metronomeControls"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(metronomeControls->sizePolicy().hasHeightForWidth());
        metronomeControls->setSizePolicy(sizePolicy1);
        metronomeControls->setMinimumSize(QSize(100, 0));
        metronomeControls->setStyleSheet(QStringLiteral(""));
        gridLayout_4 = new QGridLayout(metronomeControls);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(12, 12, 12, 6);
        soloButton = new QPushButton(metronomeControls);
        soloButton->setObjectName(QStringLiteral("soloButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(soloButton->sizePolicy().hasHeightForWidth());
        soloButton->setSizePolicy(sizePolicy2);
        soloButton->setMaximumSize(QSize(50, 16777215));
        soloButton->setLayoutDirection(Qt::LeftToRight);
        soloButton->setCheckable(true);

        gridLayout_4->addWidget(soloButton, 3, 4, 1, 1, Qt::AlignHCenter);

        horizontalSpacer_3 = new QSpacerItem(15, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_3, 1, 3, 1, 1);

        muteButton = new QPushButton(metronomeControls);
        muteButton->setObjectName(QStringLiteral("muteButton"));
        muteButton->setEnabled(true);
        sizePolicy2.setHeightForWidth(muteButton->sizePolicy().hasHeightForWidth());
        muteButton->setSizePolicy(sizePolicy2);
        muteButton->setMaximumSize(QSize(50, 16777215));
        muteButton->setLayoutDirection(Qt::LeftToRight);
        muteButton->setIconSize(QSize(0, 0));
        muteButton->setCheckable(true);

        gridLayout_4->addWidget(muteButton, 2, 4, 1, 1, Qt::AlignHCenter);

        panPanel = new QWidget(metronomeControls);
        panPanel->setObjectName(QStringLiteral("panPanel"));
        sizePolicy2.setHeightForWidth(panPanel->sizePolicy().hasHeightForWidth());
        panPanel->setSizePolicy(sizePolicy2);
        panPanel->setMaximumSize(QSize(70, 16777215));
        horizontalLayout_2 = new QHBoxLayout(panPanel);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        labelPanL = new QLabel(panPanel);
        labelPanL->setObjectName(QStringLiteral("labelPanL"));
        sizePolicy2.setHeightForWidth(labelPanL->sizePolicy().hasHeightForWidth());
        labelPanL->setSizePolicy(sizePolicy2);
        labelPanL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labelPanL);

        panSlider = new QSlider(panPanel);
        panSlider->setObjectName(QStringLiteral("panSlider"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
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

        horizontalLayout_2->addWidget(panSlider);

        labelPanR = new QLabel(panPanel);
        labelPanR->setObjectName(QStringLiteral("labelPanR"));
        sizePolicy2.setHeightForWidth(labelPanR->sizePolicy().hasHeightForWidth());
        labelPanR->setSizePolicy(sizePolicy2);
        labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labelPanR);


        gridLayout_4->addWidget(panPanel, 1, 4, 1, 1);

        peaks = new QWidget(metronomeControls);
        peaks->setObjectName(QStringLiteral("peaks"));
        horizontalLayout = new QHBoxLayout(peaks);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        peakMeterLeft = new PeakMeter(peaks);
        peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(peakMeterLeft->sizePolicy().hasHeightForWidth());
        peakMeterLeft->setSizePolicy(sizePolicy4);
        peakMeterLeft->setMinimumSize(QSize(5, 0));
        peakMeterLeft->setStyleSheet(QStringLiteral(""));

        horizontalLayout->addWidget(peakMeterLeft);

        peakMeterRight = new PeakMeter(peaks);
        peakMeterRight->setObjectName(QStringLiteral("peakMeterRight"));
        sizePolicy4.setHeightForWidth(peakMeterRight->sizePolicy().hasHeightForWidth());
        peakMeterRight->setSizePolicy(sizePolicy4);
        peakMeterRight->setMinimumSize(QSize(5, 0));

        horizontalLayout->addWidget(peakMeterRight);


        gridLayout_4->addWidget(peaks, 0, 0, 4, 1);

        levelSlider = new QSlider(metronomeControls);
        levelSlider->setObjectName(QStringLiteral("levelSlider"));
        sizePolicy4.setHeightForWidth(levelSlider->sizePolicy().hasHeightForWidth());
        levelSlider->setSizePolicy(sizePolicy4);
        levelSlider->setMaximum(120);
        levelSlider->setValue(100);
        levelSlider->setOrientation(Qt::Vertical);
        levelSlider->setTickPosition(QSlider::TicksBelow);
        levelSlider->setTickInterval(0);

        gridLayout_4->addWidget(levelSlider, 0, 2, 4, 1);

        labelMetronomeIcon = new QLabel(metronomeControls);
        labelMetronomeIcon->setObjectName(QStringLiteral("labelMetronomeIcon"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(labelMetronomeIcon->sizePolicy().hasHeightForWidth());
        labelMetronomeIcon->setSizePolicy(sizePolicy5);
        labelMetronomeIcon->setPixmap(QPixmap(QString::fromUtf8(":/images/metronome.png")));

        gridLayout_4->addWidget(labelMetronomeIcon, 0, 4, 1, 1, Qt::AlignHCenter);


        verticalLayout->addWidget(metronomeControls);


        gridLayout->addWidget(widget, 0, 7, 4, 1);

        intervalPanel = new IntervalProgressDisplay(NinjamPanel);
        intervalPanel->setObjectName(QStringLiteral("intervalPanel"));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(1);
        sizePolicy6.setVerticalStretch(1);
        sizePolicy6.setHeightForWidth(intervalPanel->sizePolicy().hasHeightForWidth());
        intervalPanel->setSizePolicy(sizePolicy6);

        gridLayout->addWidget(intervalPanel, 0, 4, 4, 1);

        panelCombos = new QWidget(NinjamPanel);
        panelCombos->setObjectName(QStringLiteral("panelCombos"));
        QSizePolicy sizePolicy7(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(panelCombos->sizePolicy().hasHeightForWidth());
        panelCombos->setSizePolicy(sizePolicy7);
        gridLayout_3 = new QGridLayout(panelCombos);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(12, 0, 12, 0);
        labelBPI = new QLabel(panelCombos);
        labelBPI->setObjectName(QStringLiteral("labelBPI"));
        labelBPI->setMargin(0);

        gridLayout_3->addWidget(labelBPI, 0, 0, 1, 1);

        comboBpi = new QComboBox(panelCombos);
        comboBpi->setObjectName(QStringLiteral("comboBpi"));
        comboBpi->setEditable(true);

        gridLayout_3->addWidget(comboBpi, 0, 1, 1, 1);

        comboBpm = new QComboBox(panelCombos);
        comboBpm->setObjectName(QStringLiteral("comboBpm"));
        comboBpm->setEditable(true);
        comboBpm->setCurrentText(QStringLiteral(""));

        gridLayout_3->addWidget(comboBpm, 1, 1, 1, 1);

        labelBPM = new QLabel(panelCombos);
        labelBPM->setObjectName(QStringLiteral("labelBPM"));

        gridLayout_3->addWidget(labelBPM, 1, 0, 1, 1);

        comboBeatsPerAccent = new QComboBox(panelCombos);
        comboBeatsPerAccent->setObjectName(QStringLiteral("comboBeatsPerAccent"));

        gridLayout_3->addWidget(comboBeatsPerAccent, 2, 1, 1, 1);

        labelAccent = new QLabel(panelCombos);
        labelAccent->setObjectName(QStringLiteral("labelAccent"));

        gridLayout_3->addWidget(labelAccent, 2, 0, 1, 1);

        labelShape = new QLabel(panelCombos);
        labelShape->setObjectName(QStringLiteral("labelShape"));

        gridLayout_3->addWidget(labelShape, 3, 0, 1, 1);

        comboShape = new QComboBox(panelCombos);
        comboShape->setObjectName(QStringLiteral("comboShape"));

        gridLayout_3->addWidget(comboShape, 3, 1, 1, 1);


        gridLayout->addWidget(panelCombos, 0, 2, 4, 1);


        retranslateUi(NinjamPanel);

        QMetaObject::connectSlotsByName(NinjamPanel);
    } // setupUi

    void retranslateUi(QWidget *NinjamPanel)
    {
        NinjamPanel->setWindowTitle(QApplication::translate("NinjamPanel", "Form", 0));
        soloButton->setText(QApplication::translate("NinjamPanel", "SOLO", 0));
        muteButton->setText(QApplication::translate("NinjamPanel", "MUTE", 0));
        labelPanL->setText(QApplication::translate("NinjamPanel", "L", 0));
        labelPanR->setText(QApplication::translate("NinjamPanel", "R", 0));
        labelMetronomeIcon->setText(QString());
#ifndef QT_NO_TOOLTIP
        labelBPI->setToolTip(QApplication::translate("NinjamPanel", "Beats per interval", 0));
#endif // QT_NO_TOOLTIP
        labelBPI->setText(QApplication::translate("NinjamPanel", "BPI", 0));
#ifndef QT_NO_TOOLTIP
        comboBpi->setToolTip(QApplication::translate("NinjamPanel", "Beats per interval", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        comboBpm->setToolTip(QApplication::translate("NinjamPanel", "Beats per minute", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        labelBPM->setToolTip(QApplication::translate("NinjamPanel", "Beats per minute", 0));
#endif // QT_NO_TOOLTIP
        labelBPM->setText(QApplication::translate("NinjamPanel", "BPM", 0));
        labelAccent->setText(QApplication::translate("NinjamPanel", "Accent", 0));
        labelShape->setText(QApplication::translate("NinjamPanel", "Shape", 0));
    } // retranslateUi

};

namespace Ui {
    class NinjamPanel: public Ui_NinjamPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NINJAMPANEL_H
