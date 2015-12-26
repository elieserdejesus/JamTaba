#ifndef CHORDPROGRESSION_H
#define CHORDPROGRESSION_H

#include <QList>

// ++++++++++++++++++++++++++++++++++++++
class Chord
{
    friend bool operator==(const Chord &c1, const Chord &c2);
public:
    Chord(QString chordText);

    Chord(QString chordText, int beat);

    inline bool hasLastPart() const
    {
        return lastPart != nullptr;
    }

    inline QString getLastPart() const
    {
        return lastPart;
    }

    inline bool hasLettersAfterRoot() const
    {
        return hasLettersAfterChordRoot;
    }

    inline QString getLettersAfterRoot() const
    {
        return lettersAfterRoot;
    }

    inline QString getBassInversion() const
    {
        return bassInversion;
    }

    inline bool hasBassInversion() const
    {
        return bassInversion != nullptr;
    }

    QString getRootKey() const;

    bool isSharp() const;

    bool isFlat() const;

    inline int getBeat() const
    {
        return beat;
    }

    inline void setBeat(int beat)
    {
        this->beat = beat;
    }

    Chord getTransposedVersion(int semitones) const;

    inline QString getChordText() const
    {
        return chordText;
    }

private:
    QString chordText;
    int beat;
    QString bassInversion;
    bool hasLettersAfterChordRoot;
    QString lettersAfterRoot;
    QString lastPart;// characters after Letters
    /*
        Example: Cmaj7(9)/Bb
        Root: C
        Bass Invertion: Bb
        Letters after root: maj
        LastPart: 7(9)
     */

    // used to transpose
    static const QString TABLE_SHARPS[12];// = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    static const QString TABLE_FLATS[12];// = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};

    static const QString *getTable(QString chordText, int semitones);

    static QString getTransposedRoot(QString rootKey, int semitones);

    void initialize(QString chordText, int beat);
};

// +++++++++++++++++++++++++++++++++++++++
class ChordProgressionMeasure
{
public:
    ChordProgressionMeasure(int beatsInTheMeasure);
    void addChord(Chord chord);
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

// ++++++++++++++++++++++++++++++++

class ChordProgression
{
public:
    ChordProgression();
    inline bool isEmpty() const
    {
        return measures.isEmpty();
    }

    void addMeasure(ChordProgressionMeasure measure);
    const QList<ChordProgressionMeasure> getMeasures() const
    {
        return measures;
    }

    QString toString() const;
    bool canBeUsed(int bpi) const;
    ChordProgression getStretchedVersion(int bpi);
    ChordProgression getTransposedVersion(int semitones);
    int getBeatsPerInterval() const;
    void clear();
private:
    QList<ChordProgressionMeasure> measures;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ChordsProgressionParser
{
public:
    virtual ChordProgression parse(QString string) = 0;
    virtual bool containsProgression(QString string) = 0;
};

// parse chord progression received as chat message
class ChatChordsProgressionParser : public ChordsProgressionParser
{
public:
    ChordProgression parse(QString string);
    bool containsProgression(QString string);
private:
    QString getSanitizedString(QString string);

    static const QString CHORD_REGEX;
    static const QString MEASURE_SEPARATORS_REGEX;
};

#endif // CHORDPROGRESSION_H
