#include "ChordsPanel.h"
#include "ui_ChordsPanel.h"

ChordsPanel::ChordsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChordsPanel),
    currentChordLabel(nullptr)
{
    ui->setupUi(this);

    //setup signals and slots
    connect(ui->buttonSendToChat, &QPushButton::clicked, this, &ChordsPanel::sendingChordsToChat);
    connect(ui->buttonDiscardChords, &QPushButton::clicked, this, &ChordsPanel::discardChords);
    connect(ui->buttonTransposeUp, &QPushButton::clicked, this, &ChordsPanel::transposeUp);
    connect(ui->buttonTransposeDown, &QPushButton::clicked, this, &ChordsPanel::transposeDown);
}

ChordsPanel::~ChordsPanel()
{
    delete ui;
}

void ChordsPanel::setChords(const ChordProgression &progression)
{
    if (progression.isEmpty())// empty progression is returned when a incompatible bpi is choosed, for example.
        return;

    clear();

    QList<ChordProgressionMeasure> measures = progression.getMeasures();
    int beatToInsert = 0;
    foreach (const ChordProgressionMeasure &measure, measures) {
        const QList<Chord> chords = measure.getChords();
        foreach (const Chord &chord, chords) {
            int chordBeat = beatToInsert;
            int chordDuration = getEstimatedChordDuration(chord, measure);
            addChord(chord, chordBeat, chordDuration);
            beatToInsert += chordDuration;
        }
    }
    this->chordProgression = progression;
}

void ChordsPanel::addChord(const Chord &chord, int beatToInsert, int durationInBeats)
{
    const int ROW = 0;
    ChordLabel *chordLabel = new ChordLabel(this, chord, durationInBeats);
    ui->gridLayout->addWidget(chordLabel, ROW, beatToInsert, 1, durationInBeats);
    chordsMap.insert(beatToInsert, chordLabel);
}

void ChordsPanel::resetGridLayout()
{
    for (int c = 0; c < ui->gridLayout->columnCount(); ++c)
        ui->gridLayout->removeItem(ui->gridLayout->itemAtPosition(0, c));
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

    foreach (ChordLabel *chordLabel, chordsMap.values()) {
        ui->gridLayout->removeWidget(chordLabel);
        delete chordLabel;
    }
    chordsMap.clear();
    currentChordLabel = nullptr;
    resetGridLayout();
}

int ChordsPanel::getEstimatedChordDuration(const Chord &chord,
                                           const ChordProgressionMeasure &measure) const
{
    int chordsInMeasure = measure.getChords().size();
    if (chordsInMeasure <= 2)// only one or two chords in the measure?
        return measure.getBeats()/chordsInMeasure;
    if (chordsInMeasure == 3 && measure.getBeats() == 4) {
        if (chord.getBeat() == 0)// first chord in the progression
            return 2;// the first chord will ocuppy 2 slots
        else
            return 1;// the last chords are 1 beat chords
    }
    return measure.getBeats();
}

void ChordsPanel::setCurrentBeat(int beat)
{
    if (chordsMap[beat]){
        currentChordLabel = chordsMap[beat]->setAsCurrentChord();
    }
    else{
        if(currentChordLabel){
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

void ChordsPanel::discardChords()
{
    clear();
    emit chordsDiscarded();
}
