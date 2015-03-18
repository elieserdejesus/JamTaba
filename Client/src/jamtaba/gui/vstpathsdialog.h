#ifndef VSTPATHSDIALOG_H
#define VSTPATHSDIALOG_H

#include <QDialog>

namespace Ui {
class VSTPathsDialog;
}

class VSTPathsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VSTPathsDialog(QWidget *parent = 0);
    ~VSTPathsDialog();

private:
    Ui::VSTPathsDialog *ui;
};

#endif // VSTPATHSDIALOG_H
