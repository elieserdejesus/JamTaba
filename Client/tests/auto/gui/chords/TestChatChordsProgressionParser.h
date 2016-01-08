#ifndef TESTCHATCHORDSPROGRESSIONPARSER_H
#define TESTCHATCHORDSPROGRESSIONPARSER_H

#include <QObject>

class TestChatChordsProgressionParser : public QObject
{
    Q_OBJECT

private slots:
    void measureSeparators_data();
    void measureSeparators();

    void measuresCount();
    void measuresCount_data();
};

#endif // TESTCHATCHORDSPROGRESSIONPARSER_H
