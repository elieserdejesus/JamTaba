#ifndef CHORDLABEL_H
#define CHORDLABEL_H

#include <QLabel>
#include "ChordProgression.h"

class QPaintEvent;

class ChordLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ChordLabel(const Chord &chord, int chordBeats);
    ~ChordLabel();
    ChordLabel *setAsCurrentChord();
    void incrementIntervalBeat();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Chord chord;
    int currentBeat;
    int beatsCount;

    static QString chordToHtmlText(const Chord &chord);

    static ChordLabel *currentChordLabel;

    void unsetAsCurrentChord();

    void setStyleSheetPropertyStatus(bool status);

    static const QColor BEAT_PROGRESS_COLOR;
};

#endif // CHORDLABEL_H
