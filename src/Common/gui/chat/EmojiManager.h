#ifndef EMOJIMANAGER_H
#define EMOJIMANAGER_H

#include <QString>
#include <QStringList>
#include <QPoint>
#include <QMap>
#include <QList>
#include <QPixmap>
#include <QAbstractItemModel>

class Emoji
{

public:
    Emoji() = default;
    Emoji(const QString &name, const QString category, uint sortOrder, const QString &unifiedCode);
    bool operator ==(const Emoji &other);

    QString category;
    QString name;
    uint sortOrder;
    QString unifiedCode;
    bool isMusical;
};

class EmojiManager
{
public:

    static const uint ICONS_SIZE;

    EmojiManager(const QString &emojisJsonPath, const QString &emojiIconsPath);

    QList<Emoji> getByCategory(const QString  &category);
    Emoji getByCode(uint emojiCode);
    QStringList getCategories() const;

    bool codeIsEmoji(uint code);
    QString emojify(const QString &string);

    QAbstractItemModel *getDataModel(int completeRole);

    QString getEmojiIconUrl(const Emoji& emoji) const;
    QString getEmojiIconUrl(const QString &emojiName) const;

    static QString emojiCodeToUtf8(const QString &emojiCode);

    void addRecent(const QString &emojiCode);
    bool hasRecents() const;
    QStringList getRecents() const;

private:

    void loadData(const QString &jsonPath);

    QMap<QString, QList<Emoji>> categorizedMap; // the category as key
    QMap<uint, Emoji> generalMap; // the emoji unicode as key

    static const QStringList categories;

    static const QMap<QString, QString> combinationsMap;

    QString iconsPath;

    QStringList recents;

};

inline QStringList EmojiManager::getRecents() const
{
    return recents;
}

inline bool EmojiManager::hasRecents() const
{
    return !recents.empty();
}

#endif // EMOJIMANAGER_H
