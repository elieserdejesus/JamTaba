#ifndef CHORDS_WIDGET_H
#define CHORDS_WIDGET_H

#include <QWidget>
#include <QList>
#include <QGridLayout>
#include <QMap>
#include "ChordProgression.h"

class ChordLabel;

// This class is used to show a sequence of ChordLabel instances organized in a gridLayout.
class ChordsWidget : public QWidget
{
public:
    ChordsWidget(QWidget *parent);
    void addChord(const Chord &chord, int durationInBeats);
    void setCurrentBeat(int beatToInsert);
    void clear();
private:
    QList<ChordLabel *> chordLabels;
    QGridLayout *gridLayout;
    void resetGridLayout();
    int beatToInsert;
    QMap<int, ChordLabel *> chordsMap; // mapping beats and chords
};

#endif // CHORDS_WIDGET_H
