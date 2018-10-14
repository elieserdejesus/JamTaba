#include "ChordsPanel.h"
#include "ui_ChordsPanel.h"

ChordsPanel::ChordsPanel(const QString &serverName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChordsPanel),
    currentChordLabel(nullptr)
{
    ui->setupUi(this);

    ui->titleLabel->setText(QString("%1 [%2]").arg(tr("Chords")).arg(serverName));

    // setup signals and slots
    connect(ui->buttonSendToChat, &QPushButton::clicked, this, &ChordsPanel::sendingChordsToChat);
    connect(ui->buttonTransposeUp, &QPushButton::clicked, this, &ChordsPanel::transposeUp);
    connect(ui->buttonTransposeDown, &QPushButton::clicked, this, &ChordsPanel::transposeDown);
    connect(ui->buttonChordsDialog, &QPushButton::clicked, this, &ChordsPanel::openingChordsDialog);
}

ChordsPanel::~ChordsPanel()
{
    delete ui;
}

void ChordsPanel::setChords(const ChordProgression &progression)
{
    if (progression.isEmpty()) // empty progression is returned when a incompatible bpi is choosed, for example.
        return;

    clear();

    chordProgression = progression;
    auto measures = chordProgression.getMeasures();
    int beatToInsert = 0;
    for (auto measure : measures) {
        auto chords = measure->getChords();

        for (auto chord : chords) {
            int chordBeat = beatToInsert;
            int chordDuration = getEstimatedChordDuration(chord, measure);
            addChord(chord, chordBeat, chordDuration);
            beatToInsert += chordDuration;
        }
    }
}

void ChordsPanel::addChord(Chord *chord, int beatToInsert, int durationInBeats)
{
    int row = beatToInsert / 16; // 4 measures per row, 16 beats per row
    int column = beatToInsert % 16;

    auto chordLabel = new ChordLabel(this, chord, durationInBeats);

    ui->gridLayout->addWidget(chordLabel, row, column, 1, durationInBeats);

    chordsMap.insert(beatToInsert, chordLabel);
}

bool ChordsPanel::setBpi(int newBpi)
{
    if (this->chordProgression.isEmpty())
        return false;

    if (newBpi != this->chordProgression.getBeatsPerInterval()) {
        if (this->chordProgression.canBeUsed(newBpi)) {
            setChords(this->chordProgression.getStretchedVersion(newBpi));
            return true;
        } else {
            clear();
        }
    }
    return false;
}

void ChordsPanel::clear()
{
    chordProgression.clear();

    for (auto chordLabel : chordsMap.values()) {
        ui->gridLayout->removeWidget(chordLabel);
        if (chordLabel) {
            chordLabel->deleteLater();
        }
    }

    chordsMap.clear();
    currentChordLabel = nullptr;
}

int ChordsPanel::getEstimatedChordDuration(Chord *chord, ChordProgressionMeasure *measure) const
{
    int chordsInMeasure = measure->getChords().size();
    if (chordsInMeasure <= 2) // only one or two chords in the measure?
        return measure->getBeats()/chordsInMeasure;
    if (chordsInMeasure == 3 && measure->getBeats() == 4) {
        if (chord->getBeat() == 0) // first chord in the progression
            return 2; // the first chord will ocuppy 2 slots
        else
            return 1; // the last chords are 1 beat chords
    }
    return measure->getBeats();
}

void ChordsPanel::setCurrentBeat(int beat)
{
    if (chordsMap[beat]) {
        currentChordLabel = chordsMap[beat]->setAsCurrentChord();
    }
    else {
        if (currentChordLabel) {
            currentChordLabel->incrementIntervalBeat();
        }
    }
}

void ChordsPanel::transposeUp()
{
    setChords(chordProgression.getTransposedVersion(1));
}

void ChordsPanel::transposeDown()
{
    setChords(chordProgression.getTransposedVersion(-1));
}
