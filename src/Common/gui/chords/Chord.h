#ifndef CHORD_H
#define CHORD_H

#include <QString>

class Chord
{
    friend bool operator==(const Chord &c1, const Chord &c2);

public:
    Chord(const QString &chordText, int beat = 0);

    void setText(const QString &chordText);

    bool hasLastPart() const;
    QString getLastPart() const;

    bool hasLettersAfterRoot() const;
    QString getLettersAfterRoot() const;

    QString getBassInversion() const;
    bool hasBassInversion() const;

    QString getRootKey() const;

    bool isSharp() const;
    bool isFlat() const;

    int getBeat() const;
    void setBeat(int beat);

    Chord getTransposedVersion(int semitones) const;

    QString getChordText() const;

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


inline bool Chord::hasLastPart() const
{
    return lastPart != nullptr;
}

inline QString Chord::getLastPart() const
{
    return lastPart;
}

inline bool Chord::hasLettersAfterRoot() const
{
    return hasLettersAfterChordRoot;
}

inline QString Chord::getLettersAfterRoot() const
{
    return lettersAfterRoot;
}

inline QString Chord::getBassInversion() const
{
    return bassInversion;
}

inline bool Chord::hasBassInversion() const
{
    return bassInversion != nullptr;
}

inline int Chord::getBeat() const
{
    return beat;
}

inline void Chord::setBeat(int beat)
{
    this->beat = beat;
}

inline QString Chord::getChordText() const
{
    return chordText;
}

#endif // CHORD_H
