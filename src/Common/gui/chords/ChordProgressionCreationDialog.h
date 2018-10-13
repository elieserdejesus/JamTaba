#ifndef CHORDPROGRESSIONCREATIONDIALOG_H
#define CHORDPROGRESSIONCREATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ChordProgressionCreationDialog;
}

class ChordProgressionCreationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChordProgressionCreationDialog(QWidget *parent = 0);
    ~ChordProgressionCreationDialog();

private:
    Ui::ChordProgressionCreationDialog *ui;
};

#endif // CHORDPROGRESSIONCREATIONDIALOG_H
