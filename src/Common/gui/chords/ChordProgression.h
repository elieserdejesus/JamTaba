#ifndef CHORDPROGRESSION_H
#define CHORDPROGRESSION_H

#include "ChordProgressionMeasure.h"
#include <QList>

class ChordProgression
{

public:
    ChordProgression();
    bool isEmpty() const;
    void addMeasure(const ChordProgressionMeasure &measure);
    const QList<ChordProgressionMeasure *> getMeasures() const;
    int getMeasuresCount() const;
    int getMaxChordsPerMeasure() const;
    QString toString() const;
    bool canBeUsed(int bpi) const;
    ChordProgression getStretchedVersion(int bpi) const;
    ChordProgression getTransposedVersion(int semitones) const;
    int getBeatsPerInterval() const;
    void clear();

private:
    QList<ChordProgressionMeasure> measures;
};

inline int ChordProgression::getMeasuresCount() const
{
    return measures.count();
}

inline bool ChordProgression::isEmpty() const
{
    return measures.isEmpty();
}

#endif // CHORDPROGRESSION_H
