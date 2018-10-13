#include <QApplication>
#include <QDebug>
#include "chords/ChordProgressionCreationDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ChordProgressionCreationDialog dialog;

    dialog.show();

    return app.exec();
}
