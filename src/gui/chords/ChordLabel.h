#ifndef CHORDLABEL_H
#define CHORDLABEL_H

#include <QLabel>
#include "ChordProgression.h"

class ChordLabel : public QLabel
{
    Q_OBJECT
public:
    ChordLabel(Chord chord );
    ~ChordLabel();
    void setAsCurrentChord();
private:
    Chord chord;
    static QString chordToHtmlText(Chord chord);

    static ChordLabel* currentChordLabel;

    void unsetAsCurrentChord();

    void setStyleSheetPropertyStatus(bool status);
};

#endif // CHORDLABEL_H
