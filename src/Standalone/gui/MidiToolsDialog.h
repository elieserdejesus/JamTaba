#ifndef MIDITOOLSDIALOG_H
#define MIDITOOLSDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class MidiToolsDialog;
}

class MidiToolsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MidiToolsDialog(const QString &lowerNote, const QString &higherNote, qint8 transpose, bool routingMidiInput);
    ~MidiToolsDialog();
    void setLearnedMidiNote(const QString &learnedNote);
    void hideMidiRoutingControls();

signals:
    void dialogClosed();
    void lowerNoteChanged(const QString &newLowerNote);
    void higherNoteChanged(const QString &newHigherNote);

    void transposeChanged(qint8 newTranspose);
    void learnMidiNoteClicked(bool); //this signal is fired when the 2 learn buttons are clicked

    void midiRoutingCheckBoxClicked(bool checkBoxIsChecked);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void lowerNoteEditionFinished();
    void higherNoteEditionFinished();
    void transposeValueChanged(int);

    void learnLowerMidiNoteToggled(bool);
    void learnHigherMidiNoteToggled(bool);
private:
    Ui::MidiToolsDialog *ui;
};

#endif // MIDITOOLSDIALOG_H
