/********************************************************************************
** Form generated from reading UI file 'pluginscandialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PluginScanDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *labelPluginText;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QDialog *PluginScanDialog)
    {
        if (PluginScanDialog->objectName().isEmpty())
            PluginScanDialog->setObjectName(QStringLiteral("PluginScanDialog"));
        PluginScanDialog->setWindowModality(Qt::NonModal);
        PluginScanDialog->resize(583, 237);
        PluginScanDialog->setMaximumSize(QSize(700, 16777215));
        verticalLayout = new QVBoxLayout(PluginScanDialog);
        verticalLayout->setSpacing(24);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        labelPluginText = new QLabel(PluginScanDialog);
        labelPluginText->setObjectName(QStringLiteral("labelPluginText"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelPluginText->sizePolicy().hasHeightForWidth());
        labelPluginText->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(labelPluginText);

        groupBox = new QGroupBox(PluginScanDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        plainTextEdit = new QPlainTextEdit(groupBox);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setAcceptDrops(false);
        plainTextEdit->setFrameShape(QFrame::NoFrame);
        plainTextEdit->setUndoRedoEnabled(false);
        plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        plainTextEdit->setReadOnly(true);
        plainTextEdit->setPlainText(QStringLiteral(""));
        plainTextEdit->setBackgroundVisible(false);

        verticalLayout_2->addWidget(plainTextEdit);


        verticalLayout->addWidget(groupBox);


        retranslateUi(PluginScanDialog);

        QMetaObject::connectSlotsByName(PluginScanDialog);
    } // setupUi

    void retranslateUi(QDialog *PluginScanDialog)
    {
        PluginScanDialog->setWindowTitle(QApplication::translate("PluginScanDialog", "[Jamtaba] Scanning plugins ...", 0));
#ifndef QT_NO_ACCESSIBILITY
        PluginScanDialog->setAccessibleDescription(QApplication::translate("PluginScanDialog", "This is the plugin scanning dialog box", 0));
#endif // QT_NO_ACCESSIBILITY
        labelPluginText->setText(QApplication::translate("PluginScanDialog", "scanning plugins...", 0));
        groupBox->setTitle(QApplication::translate("PluginScanDialog", "Founded plugins:", 0));
#ifndef QT_NO_ACCESSIBILITY
        plainTextEdit->setAccessibleDescription(QApplication::translate("PluginScanDialog", "This is the plugin founded list box", 0));
#endif // QT_NO_ACCESSIBILITY
        plainTextEdit->setPlaceholderText(QApplication::translate("PluginScanDialog", "No plugins founded!", 0));
    } // retranslateUi

};

namespace Ui {
    class PluginScanDialog: public Ui_PluginScanDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUGINSCANDIALOG_H
