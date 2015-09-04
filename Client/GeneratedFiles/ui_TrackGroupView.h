/********************************************************************************
** Form generated from reading UI file 'TrackGroupView.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKGROUPVIEW_H
#define UI_TRACKGROUPVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackGroupView
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *topPanel;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *groupNameField;
    QWidget *tracksPanel;
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *TrackGroupView)
    {
        if (TrackGroupView->objectName().isEmpty())
            TrackGroupView->setObjectName(QStringLiteral("TrackGroupView"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TrackGroupView->sizePolicy().hasHeightForWidth());
        TrackGroupView->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(TrackGroupView);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        topPanel = new QWidget(TrackGroupView);
        topPanel->setObjectName(QStringLiteral("topPanel"));
        horizontalLayout_2 = new QHBoxLayout(topPanel);
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(3, 3, 3, 3);
        groupNameField = new QLineEdit(topPanel);
        groupNameField->setObjectName(QStringLiteral("groupNameField"));
        groupNameField->setReadOnly(false);

        horizontalLayout_2->addWidget(groupNameField);


        verticalLayout->addWidget(topPanel);

        tracksPanel = new QWidget(TrackGroupView);
        tracksPanel->setObjectName(QStringLiteral("tracksPanel"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(tracksPanel->sizePolicy().hasHeightForWidth());
        tracksPanel->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(tracksPanel);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

        verticalLayout->addWidget(tracksPanel);


        retranslateUi(TrackGroupView);

        QMetaObject::connectSlotsByName(TrackGroupView);
    } // setupUi

    void retranslateUi(QWidget *TrackGroupView)
    {
        TrackGroupView->setWindowTitle(QApplication::translate("TrackGroupView", "Form", 0));
        groupNameField->setText(QApplication::translate("TrackGroupView", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class TrackGroupView: public Ui_TrackGroupView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACKGROUPVIEW_H
