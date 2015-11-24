/********************************************************************************
** Form generated from reading UI file 'NinjamPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
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
#include <QtWidgets/QFrame>
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
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QWidget *metronomeControls;
    QHBoxLayout *horizontalLayout_4;
    QWidget *peaks;
    QHBoxLayout *horizontalLayout;
    PeakMeter *peakMeterLeft;
    PeakMeter *peakMeterRight;
    QSlider *levelSlider;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QLabel *labelMetronomeIcon;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelPanL;
    QDial *panSlider;
    QLabel *labelPanR;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *muteButton;
    QPushButton *soloButton;
    IntervalProgressDisplay *intervalPanel;
    QWidget *panelCombos;
    QGridLayout *gridLayout_3;
    QLabel *labelBPI;
    QComboBox *comboBpi;
    QLabel *labelBPM;
    QComboBox *comboBpm;
    QLabel *labelAccent;
    QComboBox *comboBeatsPerAccent;
    QLabel *labelShape;
    QComboBox *comboShape;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *NinjamPanel)
    {
        if (NinjamPanel->objectName().isEmpty())
            NinjamPanel->setObjectName(QStringLiteral("NinjamPanel"));
        NinjamPanel->resize(719, 112);
        gridLayout = new QGridLayout(NinjamPanel);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
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
        metronomeControls->setStyleSheet(QStringLiteral(""));
        horizontalLayout_4 = new QHBoxLayout(metronomeControls);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(12, 6, 12, 6);
        peaks = new QWidget(metronomeControls);
        peaks->setObjectName(QStringLiteral("peaks"));
        horizontalLayout = new QHBoxLayout(peaks);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        peakMeterLeft = new PeakMeter(peaks);
        peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(peakMeterLeft->sizePolicy().hasHeightForWidth());
        peakMeterLeft->setSizePolicy(sizePolicy1);
        peakMeterLeft->setMinimumSize(QSize(5, 0));
        peakMeterLeft->setStyleSheet(QStringLiteral(""));

        horizontalLayout->addWidget(peakMeterLeft);

        peakMeterRight = new PeakMeter(peaks);
        peakMeterRight->setObjectName(QStringLiteral("peakMeterRight"));
        sizePolicy1.setHeightForWidth(peakMeterRight->sizePolicy().hasHeightForWidth());
        peakMeterRight->setSizePolicy(sizePolicy1);
        peakMeterRight->setMinimumSize(QSize(5, 0));

        horizontalLayout->addWidget(peakMeterRight);


        horizontalLayout_4->addWidget(peaks);

        levelSlider = new QSlider(metronomeControls);
        levelSlider->setObjectName(QStringLiteral("levelSlider"));
        sizePolicy1.setHeightForWidth(levelSlider->sizePolicy().hasHeightForWidth());
        levelSlider->setSizePolicy(sizePolicy1);
        levelSlider->setMaximum(160);
        levelSlider->setValue(100);
        levelSlider->setOrientation(Qt::Vertical);
        levelSlider->setTickPosition(QSlider::TicksBelow);
        levelSlider->setTickInterval(0);

        horizontalLayout_4->addWidget(levelSlider);

        frame = new QFrame(metronomeControls);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        labelMetronomeIcon = new QLabel(frame);
        labelMetronomeIcon->setObjectName(QStringLiteral("labelMetronomeIcon"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelMetronomeIcon->sizePolicy().hasHeightForWidth());
        labelMetronomeIcon->setSizePolicy(sizePolicy2);
        labelMetronomeIcon->setMinimumSize(QSize(32, 32));
        labelMetronomeIcon->setPixmap(QPixmap(QString::fromUtf8(":/images/metronome.png")));

        verticalLayout_2->addWidget(labelMetronomeIcon, 0, Qt::AlignHCenter);

        widget_2 = new QWidget(frame);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy3);
        widget_2->setMinimumSize(QSize(0, 32));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        labelPanL = new QLabel(widget_2);
        labelPanL->setObjectName(QStringLiteral("labelPanL"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(labelPanL->sizePolicy().hasHeightForWidth());
        labelPanL->setSizePolicy(sizePolicy4);
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
        sizePolicy4.setHeightForWidth(labelPanR->sizePolicy().hasHeightForWidth());
        labelPanR->setSizePolicy(sizePolicy4);
        labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labelPanR, 0, Qt::AlignBottom);


        verticalLayout_2->addWidget(widget_2);

        widget_3 = new QWidget(frame);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        sizePolicy2.setHeightForWidth(widget_3->sizePolicy().hasHeightForWidth());
        widget_3->setSizePolicy(sizePolicy2);
        widget_3->setMinimumSize(QSize(0, 24));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        muteButton = new QPushButton(widget_3);
        muteButton->setObjectName(QStringLiteral("muteButton"));
        muteButton->setEnabled(true);
        muteButton->setText(QStringLiteral("M"));
        muteButton->setCheckable(true);

        horizontalLayout_3->addWidget(muteButton, 0, Qt::AlignHCenter|Qt::AlignBottom);

        soloButton = new QPushButton(widget_3);
        soloButton->setObjectName(QStringLiteral("soloButton"));
        sizePolicy2.setHeightForWidth(soloButton->sizePolicy().hasHeightForWidth());
        soloButton->setSizePolicy(sizePolicy2);
        soloButton->setMaximumSize(QSize(16777215, 24));
        soloButton->setLayoutDirection(Qt::LeftToRight);
        soloButton->setText(QStringLiteral("S"));
        soloButton->setCheckable(true);

        horizontalLayout_3->addWidget(soloButton, 0, Qt::AlignHCenter|Qt::AlignBottom);


        verticalLayout_2->addWidget(widget_3);


        horizontalLayout_4->addWidget(frame);


        verticalLayout->addWidget(metronomeControls);


        gridLayout->addWidget(widget, 0, 7, 4, 1);

        intervalPanel = new IntervalProgressDisplay(NinjamPanel);
        intervalPanel->setObjectName(QStringLiteral("intervalPanel"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(1);
        sizePolicy5.setVerticalStretch(1);
        sizePolicy5.setHeightForWidth(intervalPanel->sizePolicy().hasHeightForWidth());
        intervalPanel->setSizePolicy(sizePolicy5);

        gridLayout->addWidget(intervalPanel, 0, 4, 4, 1);

        panelCombos = new QWidget(NinjamPanel);
        panelCombos->setObjectName(QStringLiteral("panelCombos"));
        gridLayout_3 = new QGridLayout(panelCombos);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(3);
        gridLayout_3->setVerticalSpacing(0);
        gridLayout_3->setContentsMargins(6, 0, 6, 0);
        labelBPI = new QLabel(panelCombos);
        labelBPI->setObjectName(QStringLiteral("labelBPI"));
        labelBPI->setMargin(0);

        gridLayout_3->addWidget(labelBPI, 0, 0, 1, 1);

        comboBpi = new QComboBox(panelCombos);
        comboBpi->setObjectName(QStringLiteral("comboBpi"));
        comboBpi->setEditable(true);

        gridLayout_3->addWidget(comboBpi, 0, 1, 1, 1);

        labelBPM = new QLabel(panelCombos);
        labelBPM->setObjectName(QStringLiteral("labelBPM"));

        gridLayout_3->addWidget(labelBPM, 1, 0, 1, 1);

        comboBpm = new QComboBox(panelCombos);
        comboBpm->setObjectName(QStringLiteral("comboBpm"));
        comboBpm->setEditable(true);
        comboBpm->setCurrentText(QStringLiteral(""));
        comboBpm->setMaxVisibleItems(40);

        gridLayout_3->addWidget(comboBpm, 1, 1, 1, 1);

        labelAccent = new QLabel(panelCombos);
        labelAccent->setObjectName(QStringLiteral("labelAccent"));

        gridLayout_3->addWidget(labelAccent, 2, 0, 1, 1);

        comboBeatsPerAccent = new QComboBox(panelCombos);
        comboBeatsPerAccent->setObjectName(QStringLiteral("comboBeatsPerAccent"));

        gridLayout_3->addWidget(comboBeatsPerAccent, 2, 1, 1, 1);

        labelShape = new QLabel(panelCombos);
        labelShape->setObjectName(QStringLiteral("labelShape"));

        gridLayout_3->addWidget(labelShape, 3, 0, 1, 1);

        comboShape = new QComboBox(panelCombos);
        comboShape->setObjectName(QStringLiteral("comboShape"));

        gridLayout_3->addWidget(comboShape, 3, 1, 1, 1);


        gridLayout->addWidget(panelCombos, 0, 2, 4, 1);

        horizontalSpacer = new QSpacerItem(10, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 3, 4, 1);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 5, 4, 1);


        retranslateUi(NinjamPanel);

        QMetaObject::connectSlotsByName(NinjamPanel);
    } // setupUi

    void retranslateUi(QWidget *NinjamPanel)
    {
        NinjamPanel->setWindowTitle(QApplication::translate("NinjamPanel", "Form", 0));
#ifndef QT_NO_ACCESSIBILITY
        levelSlider->setAccessibleDescription(QApplication::translate("NinjamPanel", "Slider to change the BPM of the metronome", 0));
#endif // QT_NO_ACCESSIBILITY
        labelMetronomeIcon->setText(QString());
        labelPanL->setText(QApplication::translate("NinjamPanel", "L", 0));
#ifndef QT_NO_ACCESSIBILITY
        panSlider->setAccessibleDescription(QApplication::translate("NinjamPanel", "This is a knob to pan the metronome sound", 0));
#endif // QT_NO_ACCESSIBILITY
        labelPanR->setText(QApplication::translate("NinjamPanel", "R", 0));
#ifndef QT_NO_ACCESSIBILITY
        muteButton->setAccessibleDescription(QApplication::translate("NinjamPanel", "Button to mute the metronome", 0));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        soloButton->setAccessibleDescription(QApplication::translate("NinjamPanel", "Button to soloing the metronome", 0));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_TOOLTIP
        labelBPI->setToolTip(QApplication::translate("NinjamPanel", "Beats per interval", 0));
#endif // QT_NO_TOOLTIP
        labelBPI->setText(QApplication::translate("NinjamPanel", "BPI", 0));
#ifndef QT_NO_TOOLTIP
        comboBpi->setToolTip(QApplication::translate("NinjamPanel", "Beats per interval", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        comboBpi->setAccessibleDescription(QApplication::translate("NinjamPanel", "Change the BPI here", 0));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_TOOLTIP
        labelBPM->setToolTip(QApplication::translate("NinjamPanel", "Beats per minute", 0));
#endif // QT_NO_TOOLTIP
        labelBPM->setText(QApplication::translate("NinjamPanel", "BPM", 0));
#ifndef QT_NO_TOOLTIP
        comboBpm->setToolTip(QApplication::translate("NinjamPanel", "Beats per minute", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        comboBpm->setAccessibleDescription(QApplication::translate("NinjamPanel", "Change the BPM here", 0));
#endif // QT_NO_ACCESSIBILITY
        labelAccent->setText(QApplication::translate("NinjamPanel", "Accent", 0));
#ifndef QT_NO_ACCESSIBILITY
        comboBeatsPerAccent->setAccessibleDescription(QApplication::translate("NinjamPanel", "Change the accent here", 0));
#endif // QT_NO_ACCESSIBILITY
        labelShape->setText(QApplication::translate("NinjamPanel", "Shape", 0));
#ifndef QT_NO_ACCESSIBILITY
        comboShape->setAccessibleDescription(QApplication::translate("NinjamPanel", "Change the shape here", 0));
#endif // QT_NO_ACCESSIBILITY
    } // retranslateUi

};

namespace Ui {
    class NinjamPanel: public Ui_NinjamPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NINJAMPANEL_H
