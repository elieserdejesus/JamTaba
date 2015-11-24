/********************************************************************************
** Form generated from reading UI file 'ChordsPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHORDSPANEL_H
#define UI_CHORDSPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include "../src/gui/chords/ChordsWidget.h"

QT_BEGIN_NAMESPACE

class Ui_ChordsPanel
{
public:
    QHBoxLayout *horizontalLayout;
    ChordsWidget *chordsWidget;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonSendToChat;
    QPushButton *buttonDiscardChords;
    QPushButton *buttonTransposeUp;
    QPushButton *buttonTransposeDown;

    void setupUi(QWidget *ChordsPanel)
    {
        if (ChordsPanel->objectName().isEmpty())
            ChordsPanel->setObjectName(QStringLiteral("ChordsPanel"));
        ChordsPanel->resize(725, 37);
        horizontalLayout = new QHBoxLayout(ChordsPanel);
        horizontalLayout->setSpacing(3);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        chordsWidget = new ChordsWidget(ChordsPanel);
        chordsWidget->setObjectName(QStringLiteral("chordsWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(chordsWidget->sizePolicy().hasHeightForWidth());
        chordsWidget->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(chordsWidget);

        horizontalSpacer = new QSpacerItem(6, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonSendToChat = new QPushButton(ChordsPanel);
        buttonSendToChat->setObjectName(QStringLiteral("buttonSendToChat"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(buttonSendToChat->sizePolicy().hasHeightForWidth());
        buttonSendToChat->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(buttonSendToChat);

        buttonDiscardChords = new QPushButton(ChordsPanel);
        buttonDiscardChords->setObjectName(QStringLiteral("buttonDiscardChords"));
        sizePolicy1.setHeightForWidth(buttonDiscardChords->sizePolicy().hasHeightForWidth());
        buttonDiscardChords->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(buttonDiscardChords);

        buttonTransposeUp = new QPushButton(ChordsPanel);
        buttonTransposeUp->setObjectName(QStringLiteral("buttonTransposeUp"));
        sizePolicy1.setHeightForWidth(buttonTransposeUp->sizePolicy().hasHeightForWidth());
        buttonTransposeUp->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(buttonTransposeUp);

        buttonTransposeDown = new QPushButton(ChordsPanel);
        buttonTransposeDown->setObjectName(QStringLiteral("buttonTransposeDown"));
        sizePolicy1.setHeightForWidth(buttonTransposeDown->sizePolicy().hasHeightForWidth());
        buttonTransposeDown->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(buttonTransposeDown);


        retranslateUi(ChordsPanel);

        QMetaObject::connectSlotsByName(ChordsPanel);
    } // setupUi

    void retranslateUi(QWidget *ChordsPanel)
    {
        ChordsPanel->setWindowTitle(QApplication::translate("ChordsPanel", "Form", 0));
#ifndef QT_NO_TOOLTIP
        buttonSendToChat->setToolTip(QApplication::translate("ChordsPanel", "send the chords to chat...", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        buttonSendToChat->setAccessibleDescription(QApplication::translate("ChordsPanel", "send the chords to chat", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonSendToChat->setText(QString());
#ifndef QT_NO_TOOLTIP
        buttonDiscardChords->setToolTip(QApplication::translate("ChordsPanel", "discard chord progression", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        buttonDiscardChords->setAccessibleDescription(QApplication::translate("ChordsPanel", "discard chord progression", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonDiscardChords->setText(QString());
#ifndef QT_NO_TOOLTIP
        buttonTransposeUp->setToolTip(QApplication::translate("ChordsPanel", "transpose up", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        buttonTransposeUp->setAccessibleDescription(QApplication::translate("ChordsPanel", "transpose up", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonTransposeUp->setText(QApplication::translate("ChordsPanel", "#", 0));
#ifndef QT_NO_TOOLTIP
        buttonTransposeDown->setToolTip(QApplication::translate("ChordsPanel", "transpose down", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        buttonTransposeDown->setAccessibleDescription(QApplication::translate("ChordsPanel", "transpose down", 0));
#endif // QT_NO_ACCESSIBILITY
        buttonTransposeDown->setText(QApplication::translate("ChordsPanel", "b", 0));
    } // retranslateUi

};

namespace Ui {
    class ChordsPanel: public Ui_ChordsPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHORDSPANEL_H
