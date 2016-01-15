#include "Chord.h"
#include <QDebug>

const QString Chord::TABLE_SHARPS[]
    = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

const QString Chord::TABLE_FLATS[]
    = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
// ++++++++++++++++++++++++++++++++++++++++++++++++++

Chord::Chord(const QString &chordText, int beat)
{
    this->chordText = chordText.trimmed();
    this->beat = beat;
    int indexOfInversionBar = this->chordText.indexOf("/");
    if (indexOfInversionBar > 0)
        bassInversion = this->chordText.right(this->chordText.size() - 1 - indexOfInversionBar);
    hasLettersAfterChordRoot = false;// min, maj, m, dim, add letters after chord root?
    int lettersBegin = (isFlat() || isSharp()) ? 2 : 1;
    int limit = hasBassInversion() ? indexOfInversionBar : this->chordText.length();
    int endOfLetters = lettersBegin;
    bool diminishedSymbol = chordText.size() > endOfLetters && chordText.at(endOfLetters) == QChar(
        0xc2b0);
    while (endOfLetters < limit
           && !(chordText.at(endOfLetters).isDigit() || diminishedSymbol
                || chordText.at(endOfLetters) == '(')) {
        endOfLetters++;
        hasLettersAfterChordRoot = true;
    }
    if (hasLettersAfterChordRoot)
        lettersAfterRoot = this->chordText.mid(lettersBegin, endOfLetters - lettersBegin);

    if (endOfLetters < limit) {
        int finalIndex = hasBassInversion() ? indexOfInversionBar : this->chordText.length();
        lastPart = this->chordText.mid(endOfLetters, finalIndex - endOfLetters);
    }
}

QString Chord::getRootKey() const
{
    int lenght = (isFlat() || isSharp()) ? 2 : 1;
    return chordText.left(lenght);
}

bool Chord::isSharp() const
{
    return chordText.length() > 1 && chordText.at(1) == '#';
}

bool Chord::isFlat() const
{
    return chordText.length() > 1 && chordText.at(1) == 'b';
}

Chord Chord::getTransposedVersion(int semitones) const
{
    if (semitones == 0)
        return Chord(getChordText(), getBeat());
    QString newRootKey = getTransposedRoot(getRootKey(), semitones);
    QString newChordText = newRootKey;
    if (hasLettersAfterRoot())
        newChordText += getLettersAfterRoot();
    if (hasLastPart())
        newChordText += getLastPart();
    if (hasBassInversion()) {
        QString newBassInversionRoot = getTransposedRoot(getBassInversion(), semitones);
        newChordText += "/" + newBassInversionRoot;
    }
    return Chord(newChordText, getBeat());
}

QString Chord::getTransposedRoot(const QString &rootKey, int semitones)
{
    QString newRootKey = "";
    int entryIndex = 0;
    const QString *table = getTable(rootKey, semitones);
    while (entryIndex < 12 && !(rootKey == table[entryIndex]))
        entryIndex++;
    if (entryIndex < 12){//valid index?
        if (semitones > 0)
            newRootKey = table[(entryIndex + semitones) % 12];
        else
            newRootKey = table[(entryIndex + 12 + semitones) % 12];
        return newRootKey;
    }
    qCritical() << "rootKey not founded in upTable: " << rootKey;
    return rootKey;// fallback to the original rootKey
}

const QString *Chord::getTable(const QString &chordName, int semitones)
{
    bool isSharp = chordName.length() > 1 && chordName.at(1) == '#';
    bool isFlat = chordName.length() > 1 && chordName.at(1) == 'b';
    if (semitones > 0)
        return (isFlat) ? TABLE_FLATS : TABLE_SHARPS;
    return (isSharp) ? TABLE_SHARPS : TABLE_FLATS;
}

// +++++++++++++++++++++++++++++++
bool operator==(const Chord &c1, const Chord &c2)
{
    if (c1.getChordText() != c2.getChordText())
        return false;
    if (c1.getBeat() != c2.getBeat())
        return false;
    return true;
}
