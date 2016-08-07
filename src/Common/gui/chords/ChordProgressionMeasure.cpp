#include "ChordProgressionMeasure.h"
#include "Chord.h"
#include <QList>

ChordProgressionMeasure::ChordProgressionMeasure(int beatsInTheMeasure) :
    beats(beatsInTheMeasure)
{

}

QList<Chord *> ChordProgressionMeasure::getChords() const
{
    QList<Chord *> chordPointers;
    for (const Chord &chord : chords)
        chordPointers.append(const_cast<Chord *>(&chord));
    return chordPointers;
}

void ChordProgressionMeasure::addChord(const Chord &chord)
{
    chords.append(chord);
    updateChordsBeats();
}

void ChordProgressionMeasure::updateChordsBeats()
{
    QList<Chord> newChordsList;
    for (int c = 0; c < chords.size(); ++c) {
        int newBeat = 0;
        if (c > 0) {// not the first chord?
            if (c == 1) {// secong chord?
                if (chords.size() == 2)// just two chords in the measure
                    newBeat = beats/2; // each chord occupied half measure
                else if (chords.size() >= beats) // example: 4 chords in the measure
                    newBeat = 1;// put the chord in the 2nd beat
            } else if (c == 2) {// 3rd chord?
                if (chords.size() == 3) {// 3 chords in the measure
                    // the first chord occupies half measure, and the last 2 chords occupied the rest
                    newBeat = beats - (beats/2 - beats/4);
                } else {// more than 3 chords in the measure
                    newBeat = 2;// assume the 3rd beat
                }
            }
        }
        newChordsList.append(Chord(chords.at(c).getChordText(), newBeat));
    }
    chords = newChordsList;
}

QString ChordProgressionMeasure::toString() const
{
    QString str = "| ";
    for (Chord chord : chords)
        str += chord.getChordText() + "    ";
    return str;
}
