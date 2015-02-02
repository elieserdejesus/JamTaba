/********************************************************************************
** Form generated from reading UI file 'audioiodialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOIODIALOG_H
#define UI_AUDIOIODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AudioIODialog
{
public:
    QLabel *asioDriverLabel;
    QComboBox *comboAsioDriver;
    QLabel *sampleRateLabel;
    QLabel *bufferSizeLabel;
    QComboBox *comboSampleRate;
    QComboBox *comboBufferSize;
    QGroupBox *groupBoxInputs;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QLabel *firstInputLabel;
    QComboBox *comboFirstInput;
    QLabel *lastInputLabel;
    QComboBox *comboLastInput;
    QGroupBox *groupBoxOutputs;
    QWidget *layoutWidget1;
    QFormLayout *formLayout_2;
    QLabel *firstOutputLabel;
    QComboBox *comboFirstOutput;
    QLabel *lastOutputLabel;
    QComboBox *comboLastOutput;
    QPushButton *okButton;

    void setupUi(QDialog *AudioIODialog)
    {
        if (AudioIODialog->objectName().isEmpty())
            AudioIODialog->setObjectName(QStringLiteral("AudioIODialog"));
        AudioIODialog->resize(401, 412);
        asioDriverLabel = new QLabel(AudioIODialog);
        asioDriverLabel->setObjectName(QStringLiteral("asioDriverLabel"));
        asioDriverLabel->setGeometry(QRect(54, 31, 56, 16));
        comboAsioDriver = new QComboBox(AudioIODialog);
        comboAsioDriver->setObjectName(QStringLiteral("comboAsioDriver"));
        comboAsioDriver->setGeometry(QRect(115, 31, 231, 20));
        sampleRateLabel = new QLabel(AudioIODialog);
        sampleRateLabel->setObjectName(QStringLiteral("sampleRateLabel"));
        sampleRateLabel->setGeometry(QRect(51, 261, 57, 16));
        bufferSizeLabel = new QLabel(AudioIODialog);
        bufferSizeLabel->setObjectName(QStringLiteral("bufferSizeLabel"));
        bufferSizeLabel->setGeometry(QRect(50, 300, 51, 16));
        comboSampleRate = new QComboBox(AudioIODialog);
        comboSampleRate->setObjectName(QStringLiteral("comboSampleRate"));
        comboSampleRate->setGeometry(QRect(114, 261, 231, 20));
        comboBufferSize = new QComboBox(AudioIODialog);
        comboBufferSize->setObjectName(QStringLiteral("comboBufferSize"));
        comboBufferSize->setGeometry(QRect(113, 300, 231, 20));
        groupBoxInputs = new QGroupBox(AudioIODialog);
        groupBoxInputs->setObjectName(QStringLiteral("groupBoxInputs"));
        groupBoxInputs->setGeometry(QRect(51, 67, 301, 81));
        groupBoxInputs->setFlat(false);
        layoutWidget = new QWidget(groupBoxInputs);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(11, 21, 281, 48));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        firstInputLabel = new QLabel(layoutWidget);
        firstInputLabel->setObjectName(QStringLiteral("firstInputLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, firstInputLabel);

        comboFirstInput = new QComboBox(layoutWidget);
        comboFirstInput->setObjectName(QStringLiteral("comboFirstInput"));

        formLayout->setWidget(0, QFormLayout::FieldRole, comboFirstInput);

        lastInputLabel = new QLabel(layoutWidget);
        lastInputLabel->setObjectName(QStringLiteral("lastInputLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lastInputLabel);

        comboLastInput = new QComboBox(layoutWidget);
        comboLastInput->setObjectName(QStringLiteral("comboLastInput"));

        formLayout->setWidget(1, QFormLayout::FieldRole, comboLastInput);

        groupBoxOutputs = new QGroupBox(AudioIODialog);
        groupBoxOutputs->setObjectName(QStringLiteral("groupBoxOutputs"));
        groupBoxOutputs->setGeometry(QRect(50, 160, 301, 81));
        layoutWidget1 = new QWidget(groupBoxOutputs);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(11, 21, 281, 48));
        formLayout_2 = new QFormLayout(layoutWidget1);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        firstOutputLabel = new QLabel(layoutWidget1);
        firstOutputLabel->setObjectName(QStringLiteral("firstOutputLabel"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, firstOutputLabel);

        comboFirstOutput = new QComboBox(layoutWidget1);
        comboFirstOutput->setObjectName(QStringLiteral("comboFirstOutput"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboFirstOutput);

        lastOutputLabel = new QLabel(layoutWidget1);
        lastOutputLabel->setObjectName(QStringLiteral("lastOutputLabel"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, lastOutputLabel);

        comboLastOutput = new QComboBox(layoutWidget1);
        comboLastOutput->setObjectName(QStringLiteral("comboLastOutput"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, comboLastOutput);

        okButton = new QPushButton(AudioIODialog);
        okButton->setObjectName(QStringLiteral("okButton"));
        okButton->setGeometry(QRect(270, 350, 75, 23));
        okButton->setDefault(false);
        okButton->setFlat(false);

        retranslateUi(AudioIODialog);
        QObject::connect(okButton, SIGNAL(released()), AudioIODialog, SLOT(close()));

        QMetaObject::connectSlotsByName(AudioIODialog);
    } // setupUi

    void retranslateUi(QDialog *AudioIODialog)
    {
        AudioIODialog->setWindowTitle(QApplication::translate("AudioIODialog", "Audio I/O", 0));
        asioDriverLabel->setText(QApplication::translate("AudioIODialog", "ASIO driver", 0));
        sampleRateLabel->setText(QApplication::translate("AudioIODialog", "Sample rate", 0));
        bufferSizeLabel->setText(QApplication::translate("AudioIODialog", "Buffer size", 0));
        groupBoxInputs->setTitle(QApplication::translate("AudioIODialog", "Inputs", 0));
        firstInputLabel->setText(QApplication::translate("AudioIODialog", "First", 0));
        lastInputLabel->setText(QApplication::translate("AudioIODialog", "Last", 0));
        groupBoxOutputs->setTitle(QApplication::translate("AudioIODialog", "Outpus", 0));
        firstOutputLabel->setText(QApplication::translate("AudioIODialog", "First", 0));
        lastOutputLabel->setText(QApplication::translate("AudioIODialog", "Last", 0));
        okButton->setText(QApplication::translate("AudioIODialog", "ok", 0));
    } // retranslateUi

};

namespace Ui {
    class AudioIODialog: public Ui_AudioIODialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOIODIALOG_H
