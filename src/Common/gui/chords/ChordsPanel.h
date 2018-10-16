#ifndef QCHORDPANEL_H
#define QCHORDPANEL_H

#include <QWidget>
#include "ChordProgression.h"
#include "ChordLabel.h"

namespace Ui {
class ChordsPanel;
}

class ChordsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChordsPanel(const QString &title, QWidget *parent = 0);
    ~ChordsPanel();
    void setChords(const ChordProgression &progression);
    bool setBpi(int bpi); // return true if the new bpi can be use with the currentChordProgression
    bool hasValidProgression() const;
    int getCurrentBpi() const;
    ChordProgression getChordProgression() const;

signals:
    void sendingChordsToChat();
    void openingChordsDialog(const ChordProgression &currentProgression);

public slots:
    void setCurrentBeat(int beat);

private slots:
    void transposeUp();
    void transposeDown();

private:
    Ui::ChordsPanel *ui;
    ChordProgression chordProgression;
    QMap<int, ChordLabel *> chordsMap; // mapping beats and chords
    ChordLabel *currentChordLabel;

    int getEstimatedChordDuration(Chord *chord, ChordProgressionMeasure *measure) const;
    void clear();
    void addChord(Chord *chord, int beatToInsert, int durationInBeats);
};

inline bool ChordsPanel::hasValidProgression() const
{
    return !chordProgression.isEmpty();
}

inline int ChordsPanel::getCurrentBpi() const
{
    return chordProgression.getBeatsPerInterval();
}

inline ChordProgression ChordsPanel::getChordProgression() const
{
    return chordProgression;
}

#endif // QCHORDPANEL_H
