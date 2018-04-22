#include "CrashReportDialog.h"
#include "ui_CrashReportDialog.h"

CrashReportDialog::CrashReportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashReportDialog)
{
    ui->setupUi(this);
    setWindowTitle("Crash report!");
}

CrashReportDialog::~CrashReportDialog()
{
    delete ui;
}

void CrashReportDialog::setLogDetails(const QStringList &logLines)
{
    for (const QString &line : logLines)
        ui->textEdit->appendPlainText(line);
}
