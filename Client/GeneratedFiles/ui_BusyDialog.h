/********************************************************************************
** Form generated from reading UI file 'BusyDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BUSYDIALOG_H
#define UI_BUSYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_BusyDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *labelGif;
    QLabel *labelText;

    void setupUi(QDialog *BusyDialog)
    {
        if (BusyDialog->objectName().isEmpty())
            BusyDialog->setObjectName(QStringLiteral("BusyDialog"));
        BusyDialog->setWindowModality(Qt::NonModal);
        BusyDialog->resize(320, 240);
        BusyDialog->setWindowOpacity(1);
        BusyDialog->setModal(false);
        verticalLayout = new QVBoxLayout(BusyDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        labelGif = new QLabel(BusyDialog);
        labelGif->setObjectName(QStringLiteral("labelGif"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(labelGif->sizePolicy().hasHeightForWidth());
        labelGif->setSizePolicy(sizePolicy);
        labelGif->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelGif, 0, Qt::AlignHCenter|Qt::AlignVCenter);

        labelText = new QLabel(BusyDialog);
        labelText->setObjectName(QStringLiteral("labelText"));
        labelText->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelText);


        retranslateUi(BusyDialog);

        QMetaObject::connectSlotsByName(BusyDialog);
    } // setupUi

    void retranslateUi(QDialog *BusyDialog)
    {
        BusyDialog->setWindowTitle(QApplication::translate("BusyDialog", "Dialog", 0));
        labelGif->setText(QString());
        labelText->setText(QApplication::translate("BusyDialog", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class BusyDialog: public Ui_BusyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BUSYDIALOG_H
