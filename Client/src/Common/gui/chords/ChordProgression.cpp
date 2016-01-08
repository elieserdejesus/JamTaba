#include "ChordProgression.h"
#include <QRegularExpression>
#include "gui/BpiUtils.h"

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++

ChordProgression::ChordProgression()
{
}

bool ChordProgression::canBeUsed(int bpi) const
{
    QList<int> dividers = BpiUtils::getBpiDividers(bpi);
    for (int divider : dividers) {
        if (divider == measures.size())
            return true;
    }
    return false;
}

void ChordProgression::addMeasure(const ChordProgressionMeasure &measure)
{
    measures.append(measure);
}

ChordProgression ChordProgression::getStretchedVersion(int bpi) const
{
    if (!canBeUsed(bpi))
        return ChordProgression();// return a empty progression
    int newbeatsPerMesure = bpi/measures.size();

    int currentBpi = getBeatsPerInterval();
    if (currentBpi <= 0)// avoiding division by zero when calculating stretchFactor
        return ChordProgression(); // invalid bpi, returning empty progression
    float strechFactor = static_cast<float>(bpi/currentBpi);

    ChordProgression stretchedProgression;
    for (ChordProgressionMeasure originalMeasure : measures) {
        ChordProgressionMeasure newMeasure(newbeatsPerMesure);
        foreach (Chord chord, originalMeasure.getChords()) {
            int newChordBeat = chord.getBeat() * strechFactor;
            newMeasure.addChord(Chord(chord.getChordText(), newChordBeat));
        }
        stretchedProgression.addMeasure(newMeasure);
    }

    return stretchedProgression;
}

ChordProgression ChordProgression::getTransposedVersion(int semitones) const
{
    ChordProgression newProgression;
    for (ChordProgressionMeasure originalMeasure : measures) {
        ChordProgressionMeasure newMeasure(originalMeasure.getBeats());
        foreach (Chord chord, originalMeasure.getChords())
            newMeasure.addChord(chord.getTransposedVersion(semitones));
        newProgression.addMeasure(newMeasure);
    }
    return newProgression;
}

int ChordProgression::getBeatsPerInterval() const
{
    if (measures.isEmpty())
        return 0;
    int beatsPerMeasure = measures.first().getBeats();
    return beatsPerMeasure * getMeasures().size();
}

QString ChordProgression::toString() const
{
    QString string;
    foreach (ChordProgressionMeasure measure, measures)
        string += measure.toString();
    return string;
}

void ChordProgression::clear()
{
    measures.clear();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
