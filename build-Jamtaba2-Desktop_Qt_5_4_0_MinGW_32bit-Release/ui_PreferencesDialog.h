/********************************************************************************
** Form generated from reading UI file 'PreferencesDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCESDIALOG_H
#define UI_PREFERENCESDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_IODialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *prefsTab;
    QWidget *tabAudio;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *asioDriverLabel;
    QComboBox *comboAsioDriver;
    QGroupBox *groupBoxInputs;
    QVBoxLayout *verticalLayout_4;
    QFormLayout *formLayout_4;
    QLabel *firstInputLabel_2;
    QComboBox *comboFirstInput;
    QLabel *lastInputLabel_2;
    QComboBox *comboLastInput;
    QGroupBox *groupBoxOutputs;
    QVBoxLayout *verticalLayout_5;
    QFormLayout *formLayout_3;
    QLabel *firstOutputLabel_2;
    QComboBox *comboFirstOutput;
    QLabel *lastOutputLabel_2;
    QComboBox *comboLastOutput;
    QHBoxLayout *horizontalLayout_2;
    QLabel *sampleRateLabel;
    QComboBox *comboSampleRate;
    QSpacerItem *sampleRateBufferSizeSpacer;
    QLabel *bufferSizeLabel;
    QComboBox *comboBufferSize;
    QWidget *tabMidi;
    QVBoxLayout *verticalLayout_7;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_5;
    QLabel *midiInputLabel;
    QComboBox *comboMidiInput;
    QSpacerItem *verticalSpacer;
    QWidget *tabVST;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *topButtonsLayout;
    QPushButton *buttonAddVstPath;
    QPushButton *buttonScanVSTs;
    QPushButton *buttonClearVstCache;
    QSpacerItem *horizontalSpacer_2;
    QFrame *line;
    QGroupBox *groupBoxPaths;
    QVBoxLayout *verticalLayout_11;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_10;
    QWidget *panelPaths;
    QVBoxLayout *verticalLayout_12;
    QSpacerItem *verticalSpacer_2;
    QWidget *widgetBottom;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;

    void setupUi(QDialog *IODialog)
    {
        if (IODialog->objectName().isEmpty())
            IODialog->setObjectName(QStringLiteral("IODialog"));
        IODialog->resize(412, 376);
        verticalLayout = new QVBoxLayout(IODialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        prefsTab = new QTabWidget(IODialog);
        prefsTab->setObjectName(QStringLiteral("prefsTab"));
        tabAudio = new QWidget();
        tabAudio->setObjectName(QStringLiteral("tabAudio"));
        verticalLayout_2 = new QVBoxLayout(tabAudio);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        widget = new QWidget(tabAudio);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_3 = new QVBoxLayout(widget);
        verticalLayout_3->setSpacing(20);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        asioDriverLabel = new QLabel(widget);
        asioDriverLabel->setObjectName(QStringLiteral("asioDriverLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(asioDriverLabel->sizePolicy().hasHeightForWidth());
        asioDriverLabel->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(asioDriverLabel);

        comboAsioDriver = new QComboBox(widget);
        comboAsioDriver->setObjectName(QStringLiteral("comboAsioDriver"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboAsioDriver->sizePolicy().hasHeightForWidth());
        comboAsioDriver->setSizePolicy(sizePolicy1);
        comboAsioDriver->setMinimumSize(QSize(0, 0));
        comboAsioDriver->setSizeIncrement(QSize(0, 0));

        horizontalLayout->addWidget(comboAsioDriver);


        verticalLayout_3->addLayout(horizontalLayout);

        groupBoxInputs = new QGroupBox(widget);
        groupBoxInputs->setObjectName(QStringLiteral("groupBoxInputs"));
        groupBoxInputs->setFlat(false);
        verticalLayout_4 = new QVBoxLayout(groupBoxInputs);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QStringLiteral("formLayout_4"));
        firstInputLabel_2 = new QLabel(groupBoxInputs);
        firstInputLabel_2->setObjectName(QStringLiteral("firstInputLabel_2"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, firstInputLabel_2);

        comboFirstInput = new QComboBox(groupBoxInputs);
        comboFirstInput->setObjectName(QStringLiteral("comboFirstInput"));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, comboFirstInput);

        lastInputLabel_2 = new QLabel(groupBoxInputs);
        lastInputLabel_2->setObjectName(QStringLiteral("lastInputLabel_2"));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, lastInputLabel_2);

        comboLastInput = new QComboBox(groupBoxInputs);
        comboLastInput->setObjectName(QStringLiteral("comboLastInput"));

        formLayout_4->setWidget(1, QFormLayout::FieldRole, comboLastInput);


        verticalLayout_4->addLayout(formLayout_4);


        verticalLayout_3->addWidget(groupBoxInputs);

        groupBoxOutputs = new QGroupBox(widget);
        groupBoxOutputs->setObjectName(QStringLiteral("groupBoxOutputs"));
        verticalLayout_5 = new QVBoxLayout(groupBoxOutputs);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        firstOutputLabel_2 = new QLabel(groupBoxOutputs);
        firstOutputLabel_2->setObjectName(QStringLiteral("firstOutputLabel_2"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, firstOutputLabel_2);

        comboFirstOutput = new QComboBox(groupBoxOutputs);
        comboFirstOutput->setObjectName(QStringLiteral("comboFirstOutput"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, comboFirstOutput);

        lastOutputLabel_2 = new QLabel(groupBoxOutputs);
        lastOutputLabel_2->setObjectName(QStringLiteral("lastOutputLabel_2"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, lastOutputLabel_2);

        comboLastOutput = new QComboBox(groupBoxOutputs);
        comboLastOutput->setObjectName(QStringLiteral("comboLastOutput"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, comboLastOutput);


        verticalLayout_5->addLayout(formLayout_3);


        verticalLayout_3->addWidget(groupBoxOutputs);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        sampleRateLabel = new QLabel(widget);
        sampleRateLabel->setObjectName(QStringLiteral("sampleRateLabel"));

        horizontalLayout_2->addWidget(sampleRateLabel);

        comboSampleRate = new QComboBox(widget);
        comboSampleRate->setObjectName(QStringLiteral("comboSampleRate"));

        horizontalLayout_2->addWidget(comboSampleRate);

        sampleRateBufferSizeSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(sampleRateBufferSizeSpacer);

        bufferSizeLabel = new QLabel(widget);
        bufferSizeLabel->setObjectName(QStringLiteral("bufferSizeLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(bufferSizeLabel->sizePolicy().hasHeightForWidth());
        bufferSizeLabel->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(bufferSizeLabel);

        comboBufferSize = new QComboBox(widget);
        comboBufferSize->setObjectName(QStringLiteral("comboBufferSize"));

        horizontalLayout_2->addWidget(comboBufferSize);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout_2->addWidget(widget);

        prefsTab->addTab(tabAudio, QString());
        tabMidi = new QWidget();
        tabMidi->setObjectName(QStringLiteral("tabMidi"));
        verticalLayout_7 = new QVBoxLayout(tabMidi);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        midiInputLabel = new QLabel(tabMidi);
        midiInputLabel->setObjectName(QStringLiteral("midiInputLabel"));
        sizePolicy.setHeightForWidth(midiInputLabel->sizePolicy().hasHeightForWidth());
        midiInputLabel->setSizePolicy(sizePolicy);

        horizontalLayout_5->addWidget(midiInputLabel);

        comboMidiInput = new QComboBox(tabMidi);
        comboMidiInput->setObjectName(QStringLiteral("comboMidiInput"));
        sizePolicy1.setHeightForWidth(comboMidiInput->sizePolicy().hasHeightForWidth());
        comboMidiInput->setSizePolicy(sizePolicy1);
        comboMidiInput->setMinimumSize(QSize(0, 20));
        comboMidiInput->setSizeIncrement(QSize(0, 0));

        horizontalLayout_5->addWidget(comboMidiInput);


        verticalLayout_6->addLayout(horizontalLayout_5);

        verticalSpacer = new QSpacerItem(20, 223, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer);


        verticalLayout_7->addLayout(verticalLayout_6);

        prefsTab->addTab(tabMidi, QString());
        tabVST = new QWidget();
        tabVST->setObjectName(QStringLiteral("tabVST"));
        verticalLayout_8 = new QVBoxLayout(tabVST);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        topButtonsLayout = new QHBoxLayout();
        topButtonsLayout->setSpacing(10);
        topButtonsLayout->setObjectName(QStringLiteral("topButtonsLayout"));
        buttonAddVstPath = new QPushButton(tabVST);
        buttonAddVstPath->setObjectName(QStringLiteral("buttonAddVstPath"));

        topButtonsLayout->addWidget(buttonAddVstPath);

        buttonScanVSTs = new QPushButton(tabVST);
        buttonScanVSTs->setObjectName(QStringLiteral("buttonScanVSTs"));

        topButtonsLayout->addWidget(buttonScanVSTs);

        buttonClearVstCache = new QPushButton(tabVST);
        buttonClearVstCache->setObjectName(QStringLiteral("buttonClearVstCache"));

        topButtonsLayout->addWidget(buttonClearVstCache);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        topButtonsLayout->addItem(horizontalSpacer_2);


        verticalLayout_8->addLayout(topButtonsLayout);

        line = new QFrame(tabVST);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_8->addWidget(line);

        groupBoxPaths = new QGroupBox(tabVST);
        groupBoxPaths->setObjectName(QStringLiteral("groupBoxPaths"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(groupBoxPaths->sizePolicy().hasHeightForWidth());
        groupBoxPaths->setSizePolicy(sizePolicy3);
        groupBoxPaths->setMinimumSize(QSize(0, 200));
        verticalLayout_11 = new QVBoxLayout(groupBoxPaths);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(9, -1, -1, -1);
        scrollArea = new QScrollArea(groupBoxPaths);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 348, 210));
        verticalLayout_10 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        panelPaths = new QWidget(scrollAreaWidgetContents);
        panelPaths->setObjectName(QStringLiteral("panelPaths"));
        sizePolicy3.setHeightForWidth(panelPaths->sizePolicy().hasHeightForWidth());
        panelPaths->setSizePolicy(sizePolicy3);
        verticalLayout_12 = new QVBoxLayout(panelPaths);
        verticalLayout_12->setSpacing(2);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        verticalLayout_12->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_2 = new QSpacerItem(20, 57, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_12->addItem(verticalSpacer_2);


        verticalLayout_10->addWidget(panelPaths);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_11->addWidget(scrollArea);


        verticalLayout_8->addWidget(groupBoxPaths);

        prefsTab->addTab(tabVST, QString());

        verticalLayout->addWidget(prefsTab);

        widgetBottom = new QWidget(IODialog);
        widgetBottom->setObjectName(QStringLiteral("widgetBottom"));
        horizontalLayout_4 = new QHBoxLayout(widgetBottom);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(280, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        okButton = new QPushButton(widgetBottom);
        okButton->setObjectName(QStringLiteral("okButton"));
        okButton->setMaximumSize(QSize(100, 16777215));
        okButton->setLayoutDirection(Qt::LeftToRight);
        okButton->setAutoFillBackground(false);
        okButton->setDefault(false);
        okButton->setFlat(false);

        horizontalLayout_4->addWidget(okButton);


        verticalLayout->addWidget(widgetBottom);


        retranslateUi(IODialog);

        prefsTab->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(IODialog);
    } // setupUi

    void retranslateUi(QDialog *IODialog)
    {
        IODialog->setWindowTitle(QApplication::translate("IODialog", "Preferences ...", 0));
        asioDriverLabel->setText(QApplication::translate("IODialog", "ASIO driver", 0));
        groupBoxInputs->setTitle(QApplication::translate("IODialog", "Inputs", 0));
        firstInputLabel_2->setText(QApplication::translate("IODialog", "First", 0));
        lastInputLabel_2->setText(QApplication::translate("IODialog", "Last", 0));
        groupBoxOutputs->setTitle(QApplication::translate("IODialog", "Outpus", 0));
        firstOutputLabel_2->setText(QApplication::translate("IODialog", "First", 0));
        lastOutputLabel_2->setText(QApplication::translate("IODialog", "Last", 0));
        sampleRateLabel->setText(QApplication::translate("IODialog", "Sample rate", 0));
        bufferSizeLabel->setText(QApplication::translate("IODialog", "Buffer size", 0));
        prefsTab->setTabText(prefsTab->indexOf(tabAudio), QApplication::translate("IODialog", "Audio", 0));
        midiInputLabel->setText(QApplication::translate("IODialog", "Midi input:", 0));
        prefsTab->setTabText(prefsTab->indexOf(tabMidi), QApplication::translate("IODialog", "MIDI", 0));
        buttonAddVstPath->setText(QApplication::translate("IODialog", "add path ...", 0));
        buttonScanVSTs->setText(QApplication::translate("IODialog", "scan", 0));
        buttonClearVstCache->setText(QApplication::translate("IODialog", "clear cache", 0));
        groupBoxPaths->setTitle(QApplication::translate("IODialog", "Plugin Paths:", 0));
        prefsTab->setTabText(prefsTab->indexOf(tabVST), QApplication::translate("IODialog", "VST", 0));
        okButton->setText(QApplication::translate("IODialog", "ok", 0));
    } // retranslateUi

};

namespace Ui {
    class IODialog: public Ui_IODialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESDIALOG_H
