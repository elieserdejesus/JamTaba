/********************************************************************************
** Form generated from reading UI file 'PreferencesDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCESDIALOG_H
#define UI_PREFERENCESDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
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
    QHBoxLayout *deviceLayout;
    QLabel *asioDriverLabel;
    QComboBox *comboAudioDevice;
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
    QHBoxLayout *sampleRateBfferSizeLayout;
    QLabel *sampleRateLabel;
    QComboBox *comboSampleRate;
    QSpacerItem *sampleRateBufferSizeSpacer;
    QSpacerItem *horizontalSpacer_3;
    QLabel *bufferSizeLabel;
    QComboBox *comboBufferSize;
    QWidget *tabMidi;
    QVBoxLayout *verticalLayout_7;
    QLabel *midiTabMessage;
    QSpacerItem *verticalSpacer;
    QWidget *midiContentPanel;
    QVBoxLayout *verticalLayoutMidiTab;
    QWidget *tabVST;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBoxVst;
    QVBoxLayout *verticalLayout_6;
    QPlainTextEdit *plainTextEdit;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *ButtonVst_Refresh;
    QPushButton *buttonClearVstCache;
    QPushButton *ButtonVST_AddToBlackList;
    QPushButton *ButtonVST_RemFromBlkList;
    QPlainTextEdit *BlackBoxText;
    QGroupBox *groupBoxPaths;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonAddVstScanFolder;
    QScrollArea *scrollArea;
    QWidget *panelScanFolders;
    QVBoxLayout *verticalLayout_10;
    QWidget *tabRecording;
    QGridLayout *gridLayout_2;
    QLineEdit *recordPathLineEdit;
    QLabel *label;
    QCheckBox *recordingCheckBox;
    QPushButton *browseRecPathButton;
    QSpacerItem *verticalSpacer_3;
    QWidget *widgetBottom;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;

    void setupUi(QDialog *IODialog)
    {
        if (IODialog->objectName().isEmpty())
            IODialog->setObjectName(QStringLiteral("IODialog"));
        IODialog->resize(480, 513);
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
        deviceLayout = new QHBoxLayout();
        deviceLayout->setSpacing(6);
        deviceLayout->setObjectName(QStringLiteral("deviceLayout"));
        deviceLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        asioDriverLabel = new QLabel(widget);
        asioDriverLabel->setObjectName(QStringLiteral("asioDriverLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(asioDriverLabel->sizePolicy().hasHeightForWidth());
        asioDriverLabel->setSizePolicy(sizePolicy);

        deviceLayout->addWidget(asioDriverLabel);

        comboAudioDevice = new QComboBox(widget);
        comboAudioDevice->setObjectName(QStringLiteral("comboAudioDevice"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboAudioDevice->sizePolicy().hasHeightForWidth());
        comboAudioDevice->setSizePolicy(sizePolicy1);
        comboAudioDevice->setMinimumSize(QSize(0, 0));
        comboAudioDevice->setSizeIncrement(QSize(0, 0));

        deviceLayout->addWidget(comboAudioDevice);


        verticalLayout_3->addLayout(deviceLayout);

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

        sampleRateBfferSizeLayout = new QHBoxLayout();
        sampleRateBfferSizeLayout->setSpacing(3);
        sampleRateBfferSizeLayout->setObjectName(QStringLiteral("sampleRateBfferSizeLayout"));
        sampleRateLabel = new QLabel(widget);
        sampleRateLabel->setObjectName(QStringLiteral("sampleRateLabel"));

        sampleRateBfferSizeLayout->addWidget(sampleRateLabel);

        comboSampleRate = new QComboBox(widget);
        comboSampleRate->setObjectName(QStringLiteral("comboSampleRate"));

        sampleRateBfferSizeLayout->addWidget(comboSampleRate);

        sampleRateBufferSizeSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        sampleRateBfferSizeLayout->addItem(sampleRateBufferSizeSpacer);

        horizontalSpacer_3 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        sampleRateBfferSizeLayout->addItem(horizontalSpacer_3);

        bufferSizeLabel = new QLabel(widget);
        bufferSizeLabel->setObjectName(QStringLiteral("bufferSizeLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(bufferSizeLabel->sizePolicy().hasHeightForWidth());
        bufferSizeLabel->setSizePolicy(sizePolicy2);

        sampleRateBfferSizeLayout->addWidget(bufferSizeLabel);

        comboBufferSize = new QComboBox(widget);
        comboBufferSize->setObjectName(QStringLiteral("comboBufferSize"));

        sampleRateBfferSizeLayout->addWidget(comboBufferSize);


        verticalLayout_3->addLayout(sampleRateBfferSizeLayout);


        verticalLayout_2->addWidget(widget);

        prefsTab->addTab(tabAudio, QString());
        tabMidi = new QWidget();
        tabMidi->setObjectName(QStringLiteral("tabMidi"));
        verticalLayout_7 = new QVBoxLayout(tabMidi);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        midiTabMessage = new QLabel(tabMidi);
        midiTabMessage->setObjectName(QStringLiteral("midiTabMessage"));
        sizePolicy.setHeightForWidth(midiTabMessage->sizePolicy().hasHeightForWidth());
        midiTabMessage->setSizePolicy(sizePolicy);

        verticalLayout_7->addWidget(midiTabMessage);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_7->addItem(verticalSpacer);

        midiContentPanel = new QWidget(tabMidi);
        midiContentPanel->setObjectName(QStringLiteral("midiContentPanel"));
        verticalLayoutMidiTab = new QVBoxLayout(midiContentPanel);
        verticalLayoutMidiTab->setObjectName(QStringLiteral("verticalLayoutMidiTab"));
        verticalLayoutMidiTab->setContentsMargins(0, 0, 0, 0);

        verticalLayout_7->addWidget(midiContentPanel);

        prefsTab->addTab(tabMidi, QString());
        tabVST = new QWidget();
        tabVST->setObjectName(QStringLiteral("tabVST"));
        gridLayout_3 = new QGridLayout(tabVST);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        groupBoxVst = new QGroupBox(tabVST);
        groupBoxVst->setObjectName(QStringLiteral("groupBoxVst"));
        verticalLayout_6 = new QVBoxLayout(groupBoxVst);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        plainTextEdit = new QPlainTextEdit(groupBoxVst);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setUndoRedoEnabled(false);
        plainTextEdit->setReadOnly(true);
        plainTextEdit->setPlaceholderText(QStringLiteral(""));

        verticalLayout_6->addWidget(plainTextEdit);

        widget_2 = new QWidget(groupBoxVst);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        ButtonVst_Refresh = new QPushButton(widget_2);
        ButtonVst_Refresh->setObjectName(QStringLiteral("ButtonVst_Refresh"));

        horizontalLayout_2->addWidget(ButtonVst_Refresh);

        buttonClearVstCache = new QPushButton(widget_2);
        buttonClearVstCache->setObjectName(QStringLiteral("buttonClearVstCache"));

        horizontalLayout_2->addWidget(buttonClearVstCache);

        ButtonVST_AddToBlackList = new QPushButton(widget_2);
        ButtonVST_AddToBlackList->setObjectName(QStringLiteral("ButtonVST_AddToBlackList"));

        horizontalLayout_2->addWidget(ButtonVST_AddToBlackList);

        ButtonVST_RemFromBlkList = new QPushButton(widget_2);
        ButtonVST_RemFromBlkList->setObjectName(QStringLiteral("ButtonVST_RemFromBlkList"));

        horizontalLayout_2->addWidget(ButtonVST_RemFromBlkList);


        verticalLayout_6->addWidget(widget_2);

        BlackBoxText = new QPlainTextEdit(groupBoxVst);
        BlackBoxText->setObjectName(QStringLiteral("BlackBoxText"));
        sizePolicy1.setHeightForWidth(BlackBoxText->sizePolicy().hasHeightForWidth());
        BlackBoxText->setSizePolicy(sizePolicy1);
        BlackBoxText->setMaximumSize(QSize(16777215, 70));

        verticalLayout_6->addWidget(BlackBoxText);


        gridLayout_3->addWidget(groupBoxVst, 1, 0, 1, 1);

        groupBoxPaths = new QGroupBox(tabVST);
        groupBoxPaths->setObjectName(QStringLiteral("groupBoxPaths"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBoxPaths->sizePolicy().hasHeightForWidth());
        groupBoxPaths->setSizePolicy(sizePolicy3);
        groupBoxPaths->setMinimumSize(QSize(0, 100));
        groupBoxPaths->setMaximumSize(QSize(16777215, 100));
        horizontalLayout = new QHBoxLayout(groupBoxPaths);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(9, 9, 9, 9);
        buttonAddVstScanFolder = new QPushButton(groupBoxPaths);
        buttonAddVstScanFolder->setObjectName(QStringLiteral("buttonAddVstScanFolder"));

        horizontalLayout->addWidget(buttonAddVstScanFolder);

        scrollArea = new QScrollArea(groupBoxPaths);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        panelScanFolders = new QWidget();
        panelScanFolders->setObjectName(QStringLiteral("panelScanFolders"));
        panelScanFolders->setGeometry(QRect(0, 0, 378, 65));
        verticalLayout_10 = new QVBoxLayout(panelScanFolders);
        verticalLayout_10->setSpacing(3);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(6, 0, 6, 3);
        scrollArea->setWidget(panelScanFolders);

        horizontalLayout->addWidget(scrollArea);


        gridLayout_3->addWidget(groupBoxPaths, 0, 0, 1, 1);

        prefsTab->addTab(tabVST, QString());
        tabRecording = new QWidget();
        tabRecording->setObjectName(QStringLiteral("tabRecording"));
        gridLayout_2 = new QGridLayout(tabRecording);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(18);
        recordPathLineEdit = new QLineEdit(tabRecording);
        recordPathLineEdit->setObjectName(QStringLiteral("recordPathLineEdit"));
        recordPathLineEdit->setEnabled(true);

        gridLayout_2->addWidget(recordPathLineEdit, 1, 1, 1, 1);

        label = new QLabel(tabRecording);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        recordingCheckBox = new QCheckBox(tabRecording);
        recordingCheckBox->setObjectName(QStringLiteral("recordingCheckBox"));

        gridLayout_2->addWidget(recordingCheckBox, 0, 0, 1, 3);

        browseRecPathButton = new QPushButton(tabRecording);
        browseRecPathButton->setObjectName(QStringLiteral("browseRecPathButton"));

        gridLayout_2->addWidget(browseRecPathButton, 1, 2, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_3, 2, 1, 1, 1);

        prefsTab->addTab(tabRecording, QString());

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
        okButton->setFlat(false);

        horizontalLayout_4->addWidget(okButton);


        verticalLayout->addWidget(widgetBottom);


        retranslateUi(IODialog);

        prefsTab->setCurrentIndex(1);
        okButton->setDefault(false);


        QMetaObject::connectSlotsByName(IODialog);
    } // setupUi

    void retranslateUi(QDialog *IODialog)
    {
        IODialog->setWindowTitle(QApplication::translate("IODialog", "Preferences ...", 0));
        asioDriverLabel->setText(QApplication::translate("IODialog", "Device", 0));
        groupBoxInputs->setTitle(QApplication::translate("IODialog", "Inputs", 0));
        firstInputLabel_2->setText(QApplication::translate("IODialog", "First", 0));
        lastInputLabel_2->setText(QApplication::translate("IODialog", "Last", 0));
        groupBoxOutputs->setTitle(QApplication::translate("IODialog", "Outpus", 0));
        firstOutputLabel_2->setText(QApplication::translate("IODialog", "First", 0));
        lastOutputLabel_2->setText(QApplication::translate("IODialog", "Last", 0));
        sampleRateLabel->setText(QApplication::translate("IODialog", "Sample rate", 0));
        bufferSizeLabel->setText(QApplication::translate("IODialog", "Buffer size", 0));
        prefsTab->setTabText(prefsTab->indexOf(tabAudio), QApplication::translate("IODialog", "Audio", 0));
        midiTabMessage->setText(QApplication::translate("IODialog", "Enable/disable your midi input devices:", 0));
        prefsTab->setTabText(prefsTab->indexOf(tabMidi), QApplication::translate("IODialog", "MIDI", 0));
        groupBoxVst->setTitle(QApplication::translate("IODialog", "Vst(i)s loaded", 0));
        ButtonVst_Refresh->setText(QApplication::translate("IODialog", "Refresh", 0));
        buttonClearVstCache->setText(QApplication::translate("IODialog", "clear cache", 0));
        ButtonVST_AddToBlackList->setText(QApplication::translate("IODialog", "Add to black List", 0));
        ButtonVST_RemFromBlkList->setText(QApplication::translate("IODialog", "Remove from blackList", 0));
        groupBoxPaths->setTitle(QApplication::translate("IODialog", "Plugin Paths:", 0));
#ifndef QT_NO_TOOLTIP
        buttonAddVstScanFolder->setToolTip(QApplication::translate("IODialog", "Add a new folder to scan ...", 0));
#endif // QT_NO_TOOLTIP
        buttonAddVstScanFolder->setText(QString());
        prefsTab->setTabText(prefsTab->indexOf(tabVST), QApplication::translate("IODialog", "VST", 0));
        label->setText(QApplication::translate("IODialog", "Record path:", 0));
        recordingCheckBox->setText(QApplication::translate("IODialog", "Save multi tracks ", 0));
        browseRecPathButton->setText(QApplication::translate("IODialog", "browse ...", 0));
        prefsTab->setTabText(prefsTab->indexOf(tabRecording), QApplication::translate("IODialog", "Recording", 0));
        okButton->setText(QApplication::translate("IODialog", "ok", 0));
    } // retranslateUi

};

namespace Ui {
    class IODialog: public Ui_IODialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESDIALOG_H
