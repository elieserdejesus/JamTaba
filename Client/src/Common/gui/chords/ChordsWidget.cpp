#include "ChordsWidget.h"
#include "ChordLabel.h"

ChordsWidget::ChordsWidget(QWidget *parent) :
    QWidget(parent),
    gridLayout(new QGridLayout()),
    beatToInsert(0)
{
    resetGridLayout();
}

void ChordsWidget::clear()
{
    beatToInsert = 0;
    foreach (ChordLabel *chordLabel, chordsMap.values()) {
        gridLayout->removeWidget(chordLabel);
        delete chordLabel;
    }
    chordsMap.clear();

    resetGridLayout();
}

void ChordsWidget::resetGridLayout()
{
    if (gridLayout)
        delete gridLayout;
    gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setHorizontalSpacing(3);
    setLayout(gridLayout);
}

void ChordsWidget::addChord(const Chord &chord, int durationInBeats)
{
    const int ROW = 0;
    ChordLabel *chordLabel = new ChordLabel(chord);
    gridLayout->addWidget(chordLabel, ROW, beatToInsert);

    gridLayout->setColumnStretch(beatToInsert, durationInBeats);

    chordsMap.insert(beatToInsert, chordLabel);

    beatToInsert += durationInBeats;
}

void ChordsWidget::setCurrentBeat(int beat)
{
    if (chordsMap[beat])
        chordsMap[beat]->setAsCurrentChord();
}
