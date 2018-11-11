#include "ChordProgression.h"
#include <QRegularExpression>
#include "gui/BpiUtils.h"

ChordProgression::ChordProgression()
{
    //
}

const QList<ChordProgressionMeasure *> ChordProgression::getMeasures() const
{
    QList<ChordProgressionMeasure *> measuresPointers;
    for (const ChordProgressionMeasure & measure : measures)
        measuresPointers.append(const_cast<ChordProgressionMeasure *>(&measure));
    return measuresPointers;
}

int ChordProgression::getMaxChordsPerMeasure() const
{
    auto maxChords = 1;
    for (auto measure : measures) {
        auto chords = measure.getChords().size();
        if (chords > maxChords)
            maxChords = chords;
    }

    return maxChords;
}

bool ChordProgression::canBeUsed(int bpi) const
{
    auto dividers = bpiUtils::getBpiDividers(bpi);
    for (uint divider : dividers) {
        if (static_cast<int>(divider) == measures.size())
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
        return ChordProgression(); // return a empty progression

    int newbeatsPerMesure = bpi/measures.size();

    int currentBpi = getBeatsPerInterval();
    if (currentBpi <= 0) // avoiding division by zero when calculating stretchFactor
        return ChordProgression(); // invalid bpi, returning empty progression

    float strechFactor = static_cast<float>(bpi/currentBpi);

    ChordProgression stretchedProgression;
    for (const ChordProgressionMeasure &originalMeasure : measures) {
        ChordProgressionMeasure newMeasure(newbeatsPerMesure);
        for (Chord *chord : originalMeasure.getChords()) {
            int newChordBeat = chord->getBeat() * strechFactor;
            newMeasure.addChord(Chord(chord->getChordText(), newChordBeat));
        }
        stretchedProgression.addMeasure(newMeasure);
    }

    return stretchedProgression;
}

ChordProgression ChordProgression::getTransposedVersion(int semitones) const
{
    ChordProgression newProgression;
    for (const ChordProgressionMeasure &originalMeasure : measures) {
        ChordProgressionMeasure newMeasure(originalMeasure.getBeats());
        for (Chord *chord : originalMeasure.getChords())
            newMeasure.addChord(chord->getTransposedVersion(semitones));
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
    for (const ChordProgressionMeasure &measure : measures) {
        string += measure.toString();
    }

    return string + "|";
}

void ChordProgression::clear()
{
    measures.clear();
}
