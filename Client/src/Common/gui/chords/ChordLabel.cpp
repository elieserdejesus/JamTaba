#include "ChordLabel.h"
#include <QVariant>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

ChordLabel *ChordLabel::currentChordLabel = nullptr;

// ++++++++++++++++++++++++++++++++++++++++++++=

ChordLabel::ChordLabel(const Chord &chord) :
    chord(chord)
{
    setText(chordToHtmlText(chord));
    setStyleSheetPropertyStatus(false);
}

ChordLabel::~ChordLabel()
{
    if (ChordLabel::currentChordLabel && ChordLabel::currentChordLabel == this)
        ChordLabel::currentChordLabel = nullptr;
}

void ChordLabel::setAsCurrentChord()
{
    if (currentChordLabel)
        currentChordLabel->unsetAsCurrentChord();
    currentChordLabel = this;
    setStyleSheetPropertyStatus(true);
}

void ChordLabel::unsetAsCurrentChord()
{
    ChordLabel::currentChordLabel = nullptr;
    setStyleSheetPropertyStatus(false);
}

void ChordLabel::setStyleSheetPropertyStatus(bool status)
{
    setProperty("current", QVariant(status));
    style()->unpolish(this);
    style()->polish(this);
}

QString ChordLabel::chordToHtmlText(const Chord &chord)
{
    QString finalString = "<strong>" + chord.getRootKey().left(1).toUpper();// the first chord letter
    if (chord.isFlat() || chord.isSharp()) {
        if (chord.getRootKey().size() > 1)// just in case
            finalString += "<sup>" + chord.getRootKey().mid(1, 1) + "</sup>";
    }
    finalString += "</strong>";
    if (chord.hasLettersAfterRoot())
        finalString += chord.getLettersAfterRoot().toLower();
    if (chord.hasLastPart())
        finalString += chord.getLastPart();
    if (chord.hasBassInversion()) {
        finalString += "/<strong>" + chord.getBassInversion().left(1);
        if (chord.getBassInversion().size() > 1) // chord inversion is flat or sharp?
            finalString += "<sup>" + chord.getBassInversion().mid(1, 1) + "</sup>";
        finalString += "</strong>";
    }
    return finalString;
}
