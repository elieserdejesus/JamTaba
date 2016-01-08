#ifndef CHORDSPROGRESSIONPARSER_H
#define CHORDSPROGRESSIONPARSER_H

#include "ChordProgression.h"

class QString;

class ChordsProgressionParser
{
public:
    virtual ChordProgression parse(const QString &string) = 0;
    virtual bool containsProgression(const QString &string) = 0;
};

#endif // CHORDSPROGRESSIONPARSER_H
