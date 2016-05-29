#ifndef TESTCHATCHORDSPROGRESSIONPARSER_H
#define TESTCHATCHORDSPROGRESSIONPARSER_H

#include <QObject>

class TestChatChordsProgressionParser : public QObject
{
    Q_OBJECT

private slots:
    void measureSeparators_data();
    void measureSeparators();

    void validMeasuresCount();
    void validMeasuresCount_data();

    void invalidMeasuresCount();
    void invalidMeasuresCount_data();

    void upperCaseText(); //test some invalid text containing upper case chords letters (issue #263)
    void upperCaseText_data();
};

#endif // TESTCHATCHORDSPROGRESSIONPARSER_H
