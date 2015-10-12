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
#include <QtWidgets/QDial>
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
    QSlider *levelSlider;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer;
    QLabel *labelMetronomeIcon;
    QPushButton *muteButton;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelPanL;
    QDial *panSlider;
    QLabel *labelPanR;
    QPushButton *soloButton;
    QWidget *peaks;
    QHBoxLayout *horizontalLayout;
    PeakMeter *peakMeterLeft;
    PeakMeter *peakMeterRight;
    QSpacerItem *verticalSpacer_2;
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
        levelSlider = new QSlider(metronomeControls);
        levelSlider->setObjectName(QStringLiteral("levelSlider"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(levelSlider->sizePolicy().hasHeightForWidth());
        levelSlider->setSizePolicy(sizePolicy2);
        levelSlider->setMaximum(120);
        levelSlider->setValue(100);
        levelSlider->setOrientation(Qt::Vertical);
        levelSlider->setTickPosition(QSlider::TicksBelow);
        levelSlider->setTickInterval(0);

        gridLayout_4->addWidget(levelSlider, 0, 1, 6, 1);

        horizontalSpacer_3 = new QSpacerItem(15, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_3, 0, 2, 6, 1);

        verticalSpacer = new QSpacerItem(20, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_4->addItem(verticalSpacer, 1, 3, 1, 3);

        labelMetronomeIcon = new QLabel(metronomeControls);
        labelMetronomeIcon->setObjectName(QStringLiteral("labelMetronomeIcon"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(labelMetronomeIcon->sizePolicy().hasHeightForWidth());
        labelMetronomeIcon->setSizePolicy(sizePolicy3);
        labelMetronomeIcon->setMinimumSize(QSize(32, 32));
        labelMetronomeIcon->setPixmap(QPixmap(QString::fromUtf8(":/images/metronome.png")));

        gridLayout_4->addWidget(labelMetronomeIcon, 0, 3, 1, 3, Qt::AlignHCenter);

        muteButton = new QPushButton(metronomeControls);
        muteButton->setObjectName(QStringLiteral("muteButton"));
        muteButton->setEnabled(true);
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(muteButton->sizePolicy().hasHeightForWidth());
        muteButton->setSizePolicy(sizePolicy4);
        muteButton->setMaximumSize(QSize(50, 16777215));
        muteButton->setLayoutDirection(Qt::LeftToRight);
        muteButton->setIconSize(QSize(0, 0));
        muteButton->setCheckable(true);

        gridLayout_4->addWidget(muteButton, 4, 4, 1, 1);

        widget_2 = new QWidget(metronomeControls);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        labelPanL = new QLabel(widget_2);
        labelPanL->setObjectName(QStringLiteral("labelPanL"));
        QSizePolicy sizePolicy5(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(labelPanL->sizePolicy().hasHeightForWidth());
        labelPanL->setSizePolicy(sizePolicy5);
        labelPanL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labelPanL, 0, Qt::AlignBottom);

        panSlider = new QDial(widget_2);
        panSlider->setObjectName(QStringLiteral("panSlider"));
        panSlider->setMaximumSize(QSize(32, 32));
        panSlider->setMinimum(-4);
        panSlider->setMaximum(4);
        panSlider->setPageStep(1);

        horizontalLayout_2->addWidget(panSlider);

        labelPanR = new QLabel(widget_2);
        labelPanR->setObjectName(QStringLiteral("labelPanR"));
        sizePolicy5.setHeightForWidth(labelPanR->sizePolicy().hasHeightForWidth());
        labelPanR->setSizePolicy(sizePolicy5);
        labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labelPanR, 0, Qt::AlignBottom);


        gridLayout_4->addWidget(widget_2, 2, 3, 1, 2);

        soloButton = new QPushButton(metronomeControls);
        soloButton->setObjectName(QStringLiteral("soloButton"));
        sizePolicy5.setHeightForWidth(soloButton->sizePolicy().hasHeightForWidth());
        soloButton->setSizePolicy(sizePolicy5);
        soloButton->setMaximumSize(QSize(50, 16777215));
        soloButton->setLayoutDirection(Qt::LeftToRight);
        soloButton->setCheckable(true);

        gridLayout_4->addWidget(soloButton, 4, 3, 1, 1);

        peaks = new QWidget(metronomeControls);
        peaks->setObjectName(QStringLiteral("peaks"));
        horizontalLayout = new QHBoxLayout(peaks);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        peakMeterLeft = new PeakMeter(peaks);
        peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
        sizePolicy2.setHeightForWidth(peakMeterLeft->sizePolicy().hasHeightForWidth());
        peakMeterLeft->setSizePolicy(sizePolicy2);
        peakMeterLeft->setMinimumSize(QSize(5, 0));
        peakMeterLeft->setStyleSheet(QStringLiteral(""));

        horizontalLayout->addWidget(peakMeterLeft);

        peakMeterRight = new PeakMeter(peaks);
        peakMeterRight->setObjectName(QStringLiteral("peakMeterRight"));
        sizePolicy2.setHeightForWidth(peakMeterRight->sizePolicy().hasHeightForWidth());
        peakMeterRight->setSizePolicy(sizePolicy2);
        peakMeterRight->setMinimumSize(QSize(5, 0));

        horizontalLayout->addWidget(peakMeterRight);


        gridLayout_4->addWidget(peaks, 0, 0, 6, 1);

        verticalSpacer_2 = new QSpacerItem(20, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_4->addItem(verticalSpacer_2, 3, 3, 1, 2);


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
        labelMetronomeIcon->setText(QString());
        muteButton->setText(QApplication::translate("NinjamPanel", "M", 0));
        labelPanL->setText(QApplication::translate("NinjamPanel", "L", 0));
        labelPanR->setText(QApplication::translate("NinjamPanel", "R", 0));
        soloButton->setText(QApplication::translate("NinjamPanel", "S", 0));
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
