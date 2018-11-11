#include <QApplication>
#include <QDebug>
#include "chords/ChordProgressionCreationDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ChordProgressionCreationDialog dialog;

    QObject::connect(&dialog, &ChordProgressionCreationDialog::newChordProgression, [=](const ChordProgression &progression){
        qDebug() << "Progression builded:" << progression.toString();
    });

    dialog.show();

    return app.exec();
}
