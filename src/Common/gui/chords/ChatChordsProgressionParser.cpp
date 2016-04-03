#include "ChatChordsProgressionParser.h"

#include <QString>
#include <QRegularExpression>

const QString ChatChordsProgressionParser::CHORD_REGEX
    = "([A-G][b#]?([ a-z0-9º()#\\+\\-]{1,8})?(/[A-G][b#]?[ ]{0,3})?)";

const QString ChatChordsProgressionParser::MEASURE_SEPARATORS_REGEX = "\\||!|I|l";

bool ChatChordsProgressionParser::containsProgression(const QString &string)
{
    QRegularExpression regex(
        "^([" + MEASURE_SEPARATORS_REGEX + "][ ]{0,2}" + CHORD_REGEX + "{1,4}){2,}");
    QString cleanedString = getSanitizedString(string);
    QRegularExpressionMatch match = regex.match(cleanedString);
    return match.hasMatch();
}

ChordProgression ChatChordsProgressionParser::parse(const QString &string)
{
    // remove the chord separator in the end of string. Some users type this last separator
    QString cleanedString = getSanitizedString(string);

    if (containsProgression(cleanedString)) {
        ChordProgression progression;
        QStringList measuresStrings
            = cleanedString.split(QRegularExpression(MEASURE_SEPARATORS_REGEX));
        QRegularExpression matcher(CHORD_REGEX);
        int beatsPerMeasure = 4;// using 4 beats as default TODO: try guess a good value based in current ninjam server BPI
        for (QString measureString : measuresStrings) {
            ChordProgressionMeasure measure(beatsPerMeasure);
            QRegularExpressionMatchIterator i = matcher.globalMatch(measureString);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                QString chordName = match.captured();
                measure.addChord(Chord(chordName));
            }
            if (!measure.isEmpty())
                progression.addMeasure(measure);
        }
        return progression;
    }
    return ChordProgression();// empty progression
}

QString ChatChordsProgressionParser::getSanitizedString(const QString &string)
{
    // check if user type a measure separator or blank space in the end of string
    int index = string.size() - 1;
    QRegularExpression matcher(MEASURE_SEPARATORS_REGEX + "| ");// including blanck space in regex
    while (matcher.match(string.mid(index, 1)).hasMatch())
        index--;
    return string.left(index + 1);
}
