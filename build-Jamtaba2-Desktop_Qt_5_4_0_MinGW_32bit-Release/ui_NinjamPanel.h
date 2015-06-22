/********************************************************************************
** Form generated from reading UI file 'NinjamPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include "FancyProgressDisplay.h"

QT_BEGIN_NAMESPACE

class Ui_NinjamPanel
{
public:
    QGridLayout *gridLayout;
    QWidget *panelCombos;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QComboBox *comboBpm;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QComboBox *comboBpi;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;
    QComboBox *comboBeatsPerAccent;
    FancyProgressDisplay *intervalPanel;
    QLabel *label_3;

    void setupUi(QWidget *NinjamPanel)
    {
        if (NinjamPanel->objectName().isEmpty())
            NinjamPanel->setObjectName(QStringLiteral("NinjamPanel"));
        NinjamPanel->resize(736, 89);
        NinjamPanel->setMaximumSize(QSize(16777215, 89));
        gridLayout = new QGridLayout(NinjamPanel);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        panelCombos = new QWidget(NinjamPanel);
        panelCombos->setObjectName(QStringLiteral("panelCombos"));
        horizontalLayout = new QHBoxLayout(panelCombos);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(panelCombos);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        comboBpm = new QComboBox(panelCombos);
        comboBpm->setObjectName(QStringLiteral("comboBpm"));
        comboBpm->setEditable(true);
        comboBpm->setCurrentText(QStringLiteral(""));

        horizontalLayout->addWidget(comboBpm);

        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel(panelCombos);
        label->setObjectName(QStringLiteral("label"));
        label->setMargin(0);

        horizontalLayout->addWidget(label);

        comboBpi = new QComboBox(panelCombos);
        comboBpi->setObjectName(QStringLiteral("comboBpi"));
        comboBpi->setEditable(true);

        horizontalLayout->addWidget(comboBpi);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        label_4 = new QLabel(panelCombos);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout->addWidget(label_4);

        comboBeatsPerAccent = new QComboBox(panelCombos);
        comboBeatsPerAccent->setObjectName(QStringLiteral("comboBeatsPerAccent"));

        horizontalLayout->addWidget(comboBeatsPerAccent);


        gridLayout->addWidget(panelCombos, 0, 1, 1, 1);

        intervalPanel = new FancyProgressDisplay(NinjamPanel);
        intervalPanel->setObjectName(QStringLiteral("intervalPanel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(intervalPanel->sizePolicy().hasHeightForWidth());
        intervalPanel->setSizePolicy(sizePolicy);
        intervalPanel->setMaximumSize(QSize(16777215, 32));

        gridLayout->addWidget(intervalPanel, 1, 0, 1, 2);

        label_3 = new QLabel(NinjamPanel);
        label_3->setObjectName(QStringLiteral("label_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);
        QFont font;
        font.setFamily(QStringLiteral("Courier"));
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);

        gridLayout->addWidget(label_3, 0, 0, 1, 1);


        retranslateUi(NinjamPanel);

        QMetaObject::connectSlotsByName(NinjamPanel);
    } // setupUi

    void retranslateUi(QWidget *NinjamPanel)
    {
        NinjamPanel->setWindowTitle(QApplication::translate("NinjamPanel", "Form", 0));
#ifndef QT_NO_TOOLTIP
        label_2->setToolTip(QApplication::translate("NinjamPanel", "Beats per minute", 0));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("NinjamPanel", "BPM", 0));
#ifndef QT_NO_TOOLTIP
        comboBpm->setToolTip(QApplication::translate("NinjamPanel", "Beats per minute", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label->setToolTip(QApplication::translate("NinjamPanel", "Beats per interval", 0));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("NinjamPanel", "BPI", 0));
#ifndef QT_NO_TOOLTIP
        comboBpi->setToolTip(QApplication::translate("NinjamPanel", "Beats per interval", 0));
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("NinjamPanel", "Accent", 0));
        label_3->setText(QApplication::translate("NinjamPanel", "Ninjam Interval", 0));
    } // retranslateUi

};

namespace Ui {
    class NinjamPanel: public Ui_NinjamPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NINJAMPANEL_H
