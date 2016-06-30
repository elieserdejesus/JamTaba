#ifndef CHAT_CHORDS_PROGRESSION_PARSER_H
#define CHAT_CHORDS_PROGRESSION_PARSER_H

#include "ChordsProgressionParser.h"
#include "ChordProgression.h"

class QString;

// parse chord progression received as chat message
class ChatChordsProgressionParser : public ChordsProgressionParser
{
public:
    ChordProgression parse(const QString &string) override;
    bool containsProgression(const QString &string) override;
    static bool isValidChord(const QString &chordString);
private:
    QString getSanitizedString(const QString &string);

    static const QString CHORD_REGEX;
    static const QString MEASURE_SEPARATORS_REGEX;
};

#endif // CHATCHORDSPROGRESSIONPARSER_H
