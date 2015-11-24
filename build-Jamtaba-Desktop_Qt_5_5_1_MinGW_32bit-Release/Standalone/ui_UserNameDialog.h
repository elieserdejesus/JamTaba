/********************************************************************************
** Form generated from reading UI file 'UserNameDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERNAMEDIALOG_H
#define UI_USERNAMEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_UserNameDialog
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *UserNameDialog)
    {
        if (UserNameDialog->objectName().isEmpty())
            UserNameDialog->setObjectName(QStringLiteral("UserNameDialog"));
        UserNameDialog->resize(358, 147);
        UserNameDialog->setModal(true);
        formLayout = new QFormLayout(UserNameDialog);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setHorizontalSpacing(6);
        formLayout->setVerticalSpacing(32);
        formLayout->setContentsMargins(36, 36, 36, 36);
        label = new QLabel(UserNameDialog);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        lineEdit = new QLineEdit(UserNameDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit);

        buttonBox = new QDialogButtonBox(UserNameDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        formLayout->setWidget(1, QFormLayout::SpanningRole, buttonBox);


        retranslateUi(UserNameDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), UserNameDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UserNameDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(UserNameDialog);
    } // setupUi

    void retranslateUi(QDialog *UserNameDialog)
    {
        UserNameDialog->setWindowTitle(QApplication::translate("UserNameDialog", "Login ...", 0));
#ifndef QT_NO_ACCESSIBILITY
        UserNameDialog->setAccessibleDescription(QApplication::translate("UserNameDialog", "This is the user name dialog box", 0));
#endif // QT_NO_ACCESSIBILITY
        label->setText(QApplication::translate("UserNameDialog", "Your user name:", 0));
#ifndef QT_NO_ACCESSIBILITY
        lineEdit->setAccessibleDescription(QApplication::translate("UserNameDialog", "Type your user name here", 0));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        buttonBox->setAccessibleDescription(QApplication::translate("UserNameDialog", "Press this button if you are ok", 0));
#endif // QT_NO_ACCESSIBILITY
    } // retranslateUi

};

namespace Ui {
    class UserNameDialog: public Ui_UserNameDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERNAMEDIALOG_H
