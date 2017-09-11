#ifndef TESTCHORDSPARSER_H
#define TESTCHORDSPARSER_H

#include <QObject>

class TestChordsParser : public QObject
{
    Q_OBJECT

private slots:
    void validChords();
    void validChords_data();

    void invalidChords();
    void invalidChords_data();
};

#endif // TESTCHORDSPARSER_H
