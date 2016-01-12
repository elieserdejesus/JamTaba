#ifndef QCHORDPANEL_H
#define QCHORDPANEL_H

#include <QWidget>
#include "ChordProgression.h"

namespace Ui {
class ChordsPanel;
}

class ChordsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChordsPanel(QWidget *parent = 0);
    ~ChordsPanel();
    void setChords(const ChordProgression &progression);
    void setCurrentBeat(int beat);
    bool setBpi(int bpi); // return true if the new bpi can be use with the currentChordProgression
    inline bool hasValidProgression() const
    {
        return !chordProgression.isEmpty();
    }

    inline int getCurrentBpi() const
    {
        return chordProgression.getBeatsPerInterval();
    }

    inline ChordProgression getChordProgression() const
    {
        return chordProgression;
    }

signals:
    void sendingChordsToChat();
    void chordsDiscarded();

public slots:
    void discardChords();

private slots:
    void on_buttonTransposeUp_clicked();
    void on_buttonTransposeDown_clicked();

private:
    Ui::ChordsPanel *ui;
    ChordProgression chordProgression;
    int getEstimatedChordDuration(const Chord &chord, const ChordProgressionMeasure &measure) const;
};

#endif // QCHORDPANEL_H
