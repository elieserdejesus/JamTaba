/********************************************************************************
** Form generated from reading UI file 'pluginscandialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLUGINSCANDIALOG_H
#define UI_PLUGINSCANDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_PluginScanDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *labelPluginText;

    void setupUi(QDialog *PluginScanDialog)
    {
        if (PluginScanDialog->objectName().isEmpty())
            PluginScanDialog->setObjectName(QStringLiteral("PluginScanDialog"));
        PluginScanDialog->setWindowModality(Qt::ApplicationModal);
        PluginScanDialog->resize(583, 41);
        horizontalLayout = new QHBoxLayout(PluginScanDialog);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        labelPluginText = new QLabel(PluginScanDialog);
        labelPluginText->setObjectName(QStringLiteral("labelPluginText"));

        horizontalLayout->addWidget(labelPluginText);


        retranslateUi(PluginScanDialog);

        QMetaObject::connectSlotsByName(PluginScanDialog);
    } // setupUi

    void retranslateUi(QDialog *PluginScanDialog)
    {
        PluginScanDialog->setWindowTitle(QApplication::translate("PluginScanDialog", "[Jamtaba] Scanning plugins ...", 0));
        labelPluginText->setText(QApplication::translate("PluginScanDialog", "scanning plugins...", 0));
    } // retranslateUi

};

namespace Ui {
    class PluginScanDialog: public Ui_PluginScanDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUGINSCANDIALOG_H
