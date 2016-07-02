#ifndef CHORDPROGRESSIONMEASURE_H
#define CHORDPROGRESSIONMEASURE_H

#include <QString>
#include <QStringList>
#include "Chord.h"

class ChordProgressionMeasure
{

public:
    explicit ChordProgressionMeasure(int beatsInTheMeasure);
    void addChord(const Chord &chord);
    QString toString() const;
    int getBeats() const;
    QList<Chord *> getChords() const;
    bool isEmpty() const;

private:
    int beats;
    QList<Chord> chords;

    void updateChordsBeats();
};


inline int ChordProgressionMeasure::getBeats() const
{
    return beats;
}

inline bool ChordProgressionMeasure::isEmpty() const
{
    return chords.isEmpty();
}


#endif // CHORDPROGRESSIONMEASURE_H
