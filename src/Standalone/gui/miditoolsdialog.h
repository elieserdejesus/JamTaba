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
    explicit MidiToolsDialog(const QString &lowerNote, const QString &higherNote);
    ~MidiToolsDialog();

signals:
    void dialogClosed();
    void lowerNoteChanged(const QString &newLowerNote);
    void higherNoteChanged(const QString &newHigherNote);

    void transposeChanged(qint8 newTranspose);
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void lowerNoteEditionFinished();
    void higherNoteEditionFinished();
    void transposeValueChanged(int);

private:
    Ui::MidiToolsDialog *ui;
};

#endif // MIDITOOLSDIALOG_H
