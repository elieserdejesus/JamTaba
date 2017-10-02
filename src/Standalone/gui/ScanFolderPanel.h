#ifndef SCAN_FOLDER_PANEL_H
#define SCAN_FOLDER_PANEL_H

#include <QWidget>
#include <QString>

class QPushButton;

class ScanFolderPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ScanFolderPanel(QString folder);

    inline QPushButton *getRemoveButton() const
    {
        return removeButton;
    }

    inline QString getScanFolder() const
    {
        return scanFolder;
    }

private:
    QPushButton *removeButton;
    QString scanFolder;
};

#endif
