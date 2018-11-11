#include "ChordProgressionCreationDialog.h"
#include "ui_ChordProgressionCreationDialog.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QSpacerItem>
#include <QDebug>

#include "gui/GuiUtils.h"

ChordProgressionCreationDialog::ChordProgressionCreationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChordProgressionCreationDialog),
    chordsLayout(nullptr)
{
    ui->setupUi(this);

    connect(ui->buttonLoad, &QPushButton::clicked, [=](){

        emit chordProgressionCreated(buildChordProgression());

        accept();
    });

    connect(ui->comboMeasures, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated), [=](const QString &newText){

        auto measures = newText.toInt();
        auto chordsPerMeasure = ui->comboChordsPerMeasure->currentText().toInt();

        setupChordSlots(measures, chordsPerMeasure);
    });

    connect(ui->comboChordsPerMeasure,  static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated), [=](const QString &newText){

        auto measures = ui->comboMeasures->currentText().toInt();
        auto chordsPerMeasure = newText.toInt();

        setupChordSlots(measures, chordsPerMeasure);
    });

    fillMeasuresCombo();
    fillChordsPerMeasureCombo();
}

ChordProgressionCreationDialog::~ChordProgressionCreationDialog()
{
    delete ui;
}

void ChordProgressionCreationDialog::setProgression(const ChordProgression &progression)
{
    if (progression.isEmpty())
        return;

    auto measures = progression.getMeasuresCount();
    auto chordsPerMeasure = progression.getMaxChordsPerMeasure();

    setupChordSlots(measures, chordsPerMeasure, progression);
}

void ChordProgressionCreationDialog::fillMeasuresCombo()
{
    ui->comboMeasures->clear();

    auto items = {"2", "4", "8", "12", "16"};

    for (auto item : items) {
        ui->comboMeasures->addItem(item);
    }
}

void ChordProgressionCreationDialog::fillChordsPerMeasureCombo()
{

    ui->comboChordsPerMeasure->clear();

    auto items = {"1", "2"};

    for (auto item : items) {
        ui->comboChordsPerMeasure->addItem(item);
    }
}

ChordProgression ChordProgressionCreationDialog::buildChordProgression()
{
    if (chordsLayout) {

        ChordProgression progression;

        auto measures = ui->comboMeasures->currentText().toInt();
        auto chordsPerMeasure = ui->comboChordsPerMeasure->currentText().toInt();

        QString lastChordText;

        for (int m = 0; m < measures; ++m) {

            ChordProgressionMeasure measure(4);

            int chordBeat = 0;
            for (int c = 0; c < chordsPerMeasure; ++c) {

                auto row = m/chordsLayout->columnCount();
                auto col = m % chordsLayout->columnCount();
                auto layoutItem = chordsLayout->itemAtPosition(row, col);

                Q_ASSERT(layoutItem);

                if (layoutItem->layout()) {

                    auto rootIndex = c < 1 ? 2 : 5; // combo position in QHBoxLayout
                    auto qualityIndex = rootIndex + 1;

                    auto comboRoot = qobject_cast<QComboBox *>(layoutItem->layout()->itemAt(rootIndex)->widget());
                    auto comboQuality = qobject_cast<QComboBox *>(layoutItem->layout()->itemAt(qualityIndex)->widget());

                    Q_ASSERT(comboRoot);
                    Q_ASSERT(comboQuality);

                    auto chordText = lastChordText;

                    auto rootIsValid = comboRoot->currentIndex() >= 0 && comboRoot->currentIndex() < comboRoot->count() - 1;
                    auto qualityIsValid = comboQuality->currentIndex() >= 0 && comboQuality->currentIndex() < comboQuality->count() - 1;

                    if (rootIsValid) { // selection is valid? (the last combo item is the 'empty')
                        QString quality;
                        if (qualityIsValid) { // valid selection ?
                            quality = comboQuality->currentText();
                        }
                        chordText = comboRoot->currentText() + quality;
                        lastChordText = chordText;
                    }

                    if (rootIsValid)
                        measure.addChord(Chord(chordText, chordBeat));

                    chordBeat += 2;
                }
            }

            progression.addMeasure(measure);
        }

        return progression;
    }

    return ChordProgression();
}

QComboBox *ChordProgressionCreationDialog::createChordRootCombo(bool emptySelection)
{

    auto chords = {"C", "C#", "D", "Db", "D#", "E", "Eb", "F", "F#", "G", "Gb", "G#", "A", "Ab", "A#", "B", "Bb", "--"};

    auto combo = new QComboBox();
    combo->setMaxVisibleItems(17);
    combo->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    for (auto chord : chords) {
        combo->addItem(chord);
    }

    if (emptySelection)
        combo->setCurrentIndex(combo->count() - 1);

    return combo;
}

QComboBox *ChordProgressionCreationDialog::createChordQualityCombo()
{
    auto combo = new QComboBox();
    combo->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    auto options = {"M", "m", "6", "m6", "6/9", "maj7", "7", "7b5", "7#5", "m7", "m(maj7)", "m7b5", "º7", "dim", "9", "9b5", "9#5", "maj9", "m9", "m11", "13", "maj13", "m13", "sus4", "sus2", "7sus4", "7sus2", "9sus4", "9sus2", "aug", "5", "--"};

    for (auto option : options) {
        combo->addItem(option);
    }

    combo->setCurrentIndex(combo->count() - 1);

    return combo;
}

void ChordProgressionCreationDialog::clearChordsLayout()
{
    if (chordsLayout) {
        gui::clearLayout(chordsLayout);
        chordsLayout->deleteLater();
    }

    chordsLayout = new QGridLayout();
    chordsLayout->setContentsMargins(0, 0, 0, 0);

    const auto chordsMargin = 60;

    chordsLayout->setHorizontalSpacing(chordsMargin);
    chordsLayout->setVerticalSpacing(chordsMargin);

    ui->verticalLayout->insertLayout(1, chordsLayout);

    ui->verticalLayout->setStretch(1, 1);
}

void ChordProgressionCreationDialog::setupChordSlots(int measures, int chordsPerMeasure, const ChordProgression &progression)
{
    if (measures <= 0 || chordsPerMeasure <= 0)
        return;

    clearChordsLayout();

    Q_ASSERT(chordsLayout);

    auto measuresPerRow = qMin(4, measures);
    auto rows = measures/measuresPerRow;

    for (int row = 0; row < rows; ++row) {
        for (int m = 0; m < measuresPerRow; ++m) {

            auto layout = new QHBoxLayout();
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(2);

            auto measureIndex = (m + (row * measuresPerRow));
            layout->addWidget(new QLabel(QString::number(measureIndex + 1)));
            layout->addSpacing(6);

            for (int chordIndex = 0; chordIndex < chordsPerMeasure; ++chordIndex) {

                auto emptySelection = chordIndex > 0; // second chords will be "empty" as default

                auto comboChordRoot = createChordRootCombo(emptySelection);
                layout->addWidget(comboChordRoot);

                auto comboChordQuality = createChordQualityCombo();
                layout->addWidget(comboChordQuality);
                layout->setAlignment(comboChordQuality, Qt::AlignLeft);

                if (!progression.isEmpty()) {
                    auto measure = progression.getMeasures().at(measureIndex);
                    if (measure) {
                        auto chords = measure->getChords();
                        if (chordIndex < chords.size())
                            fillChordCombos(comboChordRoot, comboChordQuality, chords.at(chordIndex));
                    }
                }

                if (chordIndex < chordsPerMeasure - 1) {
                    layout->addSpacing(chordsLayout->spacing()/3);
                }
            }

            chordsLayout->addLayout(layout, row, m, 1, 1);
            chordsLayout->setAlignment(layout, Qt::AlignLeft);

            auto lastWidget = layout->itemAt(layout->count()-1)->widget();
            QWidget::setTabOrder(lastWidget, ui->buttonLoad); // keep the 'ok' button as the last element in tab order
        }
    }

    ui->comboMeasures->setCurrentText(QString::number(measures));
    ui->comboChordsPerMeasure->setCurrentText(QString::number(chordsPerMeasure));

    QApplication::processEvents();
    adjustSize();
}

void ChordProgressionCreationDialog::fillChordCombos(QComboBox *rootCombo, QComboBox *qualityCombo, Chord *chord)
{
    rootCombo->setCurrentText(chord->getRootKey());

    auto quality = chord->getLettersAfterRoot();
    if (!quality.isEmpty())
        qualityCombo->setCurrentText(quality);
}

int ChordProgressionCreationDialog::guessMeasures(int bpi) const
{
    switch (bpi) {
        case 64:
        case 32:
        case 16:
        case 8 : return bpi/4;   // 4/4 measures

        case 12:
        case 24: return bpi/3;   // 3/4 measures

        case 48: return 12;      // 12 bars blues

        case 20:
        case 40: return bpi/5;   // 5/4 measures

        case 28:
        case 56: return bpi/7;   // 7/4 measures

    }

    return 8; // 8 measures as default
}

void ChordProgressionCreationDialog::show(int currentBpi, const ChordProgression &currentProgression)
{
    auto measures = currentProgression.isEmpty() ? guessMeasures(currentBpi) : currentProgression.getMeasuresCount();
    auto chordsPerMeasure = currentProgression.isEmpty() ? 1 : currentProgression.getMaxChordsPerMeasure();

    setupChordSlots(measures, chordsPerMeasure, currentProgression);

    QDialog::show();
    raise();
    activateWindow();
}
