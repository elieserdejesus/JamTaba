#ifndef CHORD_H
#define CHORD_H

#include <QString>

class Chord
{
    friend bool operator==(const Chord &c1, const Chord &c2);

public:
    Chord(const QString &chordText, int beat = 0);

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

    static const QString *getTable(const QString &chordText, int semitones);

    static QString getTransposedRoot(const QString &rootKey, int semitones);
};

#endif // CHORD_H
