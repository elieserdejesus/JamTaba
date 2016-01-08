#include "ChordsPanel.h"
#include "ui_ChordsPanel.h"
#include "ChordsWidget.h"

ChordsPanel::ChordsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChordsPanel)
{
    ui->setupUi(this);

    QObject::connect(ui->buttonSendToChat, SIGNAL(clicked(bool)), this,
                     SIGNAL(buttonSendChordsToChatClicked()));

    QObject::connect(ui->buttonDiscardChords, SIGNAL(clicked(bool)), this,
                     SLOT(on_buttonDiscardChordsClicked()));
}

ChordsPanel::~ChordsPanel()
{
    delete ui;
}

void ChordsPanel::setChords(const ChordProgression &progression)
{
    ui->chordsWidget->clear();
    this->chordProgression.clear();
    if (progression.isEmpty())// empty progression is returned when a incompatible bpi is choosed, for example.
        return;
    QList<ChordProgressionMeasure> measures = progression.getMeasures();
    foreach (ChordProgressionMeasure measure, measures) {
        const QList<Chord> chords = measure.getChords();
        foreach (Chord chord, chords)
            ui->chordsWidget->addChord(chord, getEstimatedChordDuration(chord, measure));
    }
    this->chordProgression = progression;
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
            ui->chordsWidget->clear();
            this->chordProgression.clear();
        }
    }
    return false;
}

int ChordsPanel::getEstimatedChordDuration(const Chord &chord, const ChordProgressionMeasure &measure) const
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
    ui->chordsWidget->setCurrentBeat(beat);
}

void ChordsPanel::on_buttonTransposeUp_clicked()
{
    setChords(chordProgression.getTransposedVersion(1));
}

void ChordsPanel::on_buttonTransposeDown_clicked()
{
    setChords(chordProgression.getTransposedVersion(-1));
}

void ChordsPanel::on_buttonDiscardChordsClicked()
{
    chordProgression.clear();
    ui->chordsWidget->clear();
    setVisible(false);
}
