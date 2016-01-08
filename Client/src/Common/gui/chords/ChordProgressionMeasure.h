#ifndef CHORDPROGRESSIONMEASURE_H
#define CHORDPROGRESSIONMEASURE_H

#include <QString>
#include <QStringList>
#include "Chord.h"

class ChordProgressionMeasure
{
public:
    ChordProgressionMeasure(int beatsInTheMeasure);
    void addChord(const Chord &chord);
    QString toString() const;
    inline int getBeats() const
    {
        return beats;
    }

    const QList<Chord> getChords() const
    {
        return chords;
    }

    inline bool isEmpty() const
    {
        return chords.isEmpty();
    }

private:
    int beats;
    QList<Chord> chords;

    void updateChordsBeats();
};

#endif // CHORDPROGRESSIONMEASURE_H
