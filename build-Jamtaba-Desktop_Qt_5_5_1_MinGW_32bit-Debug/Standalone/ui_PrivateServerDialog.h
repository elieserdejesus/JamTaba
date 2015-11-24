/********************************************************************************
** Form generated from reading UI file 'PrivateServerDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRIVATESERVERDIALOG_H
#define UI_PRIVATESERVERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PrivateServerDialog
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *serverTextField;
    QLabel *label_2;
    QLineEdit *passwordTextField;
    QLabel *label_3;
    QLineEdit *serverPortTextField;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;

    void setupUi(QDialog *PrivateServerDialog)
    {
        if (PrivateServerDialog->objectName().isEmpty())
            PrivateServerDialog->setObjectName(QStringLiteral("PrivateServerDialog"));
        PrivateServerDialog->setWindowModality(Qt::ApplicationModal);
        PrivateServerDialog->resize(303, 155);
        PrivateServerDialog->setModal(true);
        verticalLayout = new QVBoxLayout(PrivateServerDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget = new QWidget(PrivateServerDialog);
        widget->setObjectName(QStringLiteral("widget"));
        formLayout = new QFormLayout(widget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        serverTextField = new QLineEdit(widget);
        serverTextField->setObjectName(QStringLiteral("serverTextField"));

        formLayout->setWidget(0, QFormLayout::FieldRole, serverTextField);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        passwordTextField = new QLineEdit(widget);
        passwordTextField->setObjectName(QStringLiteral("passwordTextField"));

        formLayout->setWidget(2, QFormLayout::FieldRole, passwordTextField);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        serverPortTextField = new QLineEdit(widget);
        serverPortTextField->setObjectName(QStringLiteral("serverPortTextField"));

        formLayout->setWidget(1, QFormLayout::FieldRole, serverPortTextField);


        verticalLayout->addWidget(widget);

        widget_2 = new QWidget(PrivateServerDialog);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(widget_2);
        okButton->setObjectName(QStringLiteral("okButton"));

        horizontalLayout->addWidget(okButton);


        verticalLayout->addWidget(widget_2);

        QWidget::setTabOrder(serverTextField, serverPortTextField);
        QWidget::setTabOrder(serverPortTextField, passwordTextField);
        QWidget::setTabOrder(passwordTextField, okButton);

        retranslateUi(PrivateServerDialog);

        QMetaObject::connectSlotsByName(PrivateServerDialog);
    } // setupUi

    void retranslateUi(QDialog *PrivateServerDialog)
    {
        PrivateServerDialog->setWindowTitle(QApplication::translate("PrivateServerDialog", "Connecting in private Ninjam server ...", 0));
#ifndef QT_NO_ACCESSIBILITY
        PrivateServerDialog->setAccessibleDescription(QApplication::translate("PrivateServerDialog", "This the private server dialog", 0));
#endif // QT_NO_ACCESSIBILITY
        label->setText(QApplication::translate("PrivateServerDialog", "Server:", 0));
#ifndef QT_NO_ACCESSIBILITY
        serverTextField->setAccessibleDescription(QApplication::translate("PrivateServerDialog", "type here the name of the server to join", 0));
#endif // QT_NO_ACCESSIBILITY
        serverTextField->setText(QApplication::translate("PrivateServerDialog", "localhost", 0));
#ifndef QT_NO_TOOLTIP
        label_2->setToolTip(QApplication::translate("PrivateServerDialog", "leave blank to connect as annonymous", 0));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("PrivateServerDialog", "Password:", 0));
#ifndef QT_NO_TOOLTIP
        passwordTextField->setToolTip(QApplication::translate("PrivateServerDialog", "leave blank to connect as annonymous", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        passwordTextField->setAccessibleDescription(QApplication::translate("PrivateServerDialog", "type here the password", 0));
#endif // QT_NO_ACCESSIBILITY
        label_3->setText(QApplication::translate("PrivateServerDialog", "Port:", 0));
#ifndef QT_NO_ACCESSIBILITY
        serverPortTextField->setAccessibleDescription(QApplication::translate("PrivateServerDialog", "type here the port to use", 0));
#endif // QT_NO_ACCESSIBILITY
        serverPortTextField->setText(QApplication::translate("PrivateServerDialog", "2049", 0));
#ifndef QT_NO_ACCESSIBILITY
        okButton->setAccessibleDescription(QApplication::translate("PrivateServerDialog", "Press this button if you are ok", 0));
#endif // QT_NO_ACCESSIBILITY
        okButton->setText(QApplication::translate("PrivateServerDialog", "OK", 0));
    } // retranslateUi

};

namespace Ui {
    class PrivateServerDialog: public Ui_PrivateServerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRIVATESERVERDIALOG_H
