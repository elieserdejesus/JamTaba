#ifndef CHORDPROGRESSIONCREATIONDIALOG_H
#define CHORDPROGRESSIONCREATIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QGridLayout>
#include "ChordProgression.h"

namespace Ui {
class ChordProgressionCreationDialog;
}

class ChordProgressionCreationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChordProgressionCreationDialog(QWidget *parent = 0);
    ~ChordProgressionCreationDialog();

public slots:
    void show(int currentBpi, const ChordProgression &currentChordProgression);
    void setProgression(const ChordProgression &progression);

signals:
    void chordProgressionCreated(const ChordProgression &newProgression);

private:
    Ui::ChordProgressionCreationDialog *ui;

    QGridLayout *chordsLayout;

    void setupChordSlots(int measures, int chordsPerMeasure, const ChordProgression &progression = ChordProgression());

    ChordProgression buildChordProgression();

    QComboBox* createChordRootCombo(bool emptySelection = false);
    QComboBox* createChordQualityCombo();

    void fillMeasuresCombo();
    void fillChordsPerMeasureCombo();

    int guessMeasures(int bpi) const;

    void fillChordCombos(QComboBox *rootCombo, QComboBox *qualityCombo, Chord *chord);

    void clearChordsLayout();
};

#endif // CHORDPROGRESSIONCREATIONDIALOG_H
