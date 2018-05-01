#ifndef TEST_EMOJI_PARSER
#define TEST_EMOJI_PARSER

#include <QObject>

class TestEmojiParser : public QObject
{
    Q_OBJECT

private slots:

    void combinationEmojis_data();
    void combinationEmojis();

    void combinationEmojisInsideMessages_data();
    void combinationEmojisInsideMessages();

};

#endif // TEST_EMOJI_PARSER
