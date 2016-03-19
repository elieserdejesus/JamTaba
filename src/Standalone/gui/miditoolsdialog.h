#ifndef MIDITOOLSDIALOG_H
#define MIDITOOLSDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class MidiToolsDialog;
}

class LocalTrackViewStandalone;

class MidiToolsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MidiToolsDialog(LocalTrackViewStandalone *trackView);
    ~MidiToolsDialog();

signals:
    void dialogClosed();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MidiToolsDialog *ui;
    LocalTrackViewStandalone *trackView;
};

#endif // MIDITOOLSDIALOG_H
