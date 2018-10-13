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

signals:
    void newChordProgression(const ChordProgression &progression);

private:
    Ui::ChordProgressionCreationDialog *ui;

    QGridLayout *chordsLayout;

    void setupChordSlots(int measures, int chordsPerMeasure);

    ChordProgression buildChordProgression();

    QComboBox* createChordRootCombo(bool emptySelection = false);
    QComboBox* createChordQualityCombo();

    void fillMeasuresCombo();
    void fillChordsPerMeasureCombo();

    void clearChordsLayout();
};

#endif // CHORDPROGRESSIONCREATIONDIALOG_H
