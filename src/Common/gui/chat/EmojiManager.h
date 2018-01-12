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

class Emojifier
{
public:
    virtual QString emojify(const QString &string) = 0;
};

class EmojiManager : public Emojifier
{
public:

    static const uint ICONS_SIZE;

    EmojiManager(const QString &emojisJsonPath, const QString &emojiIconsPath);

    QList<Emoji> getByCategory(const QString  &category) const;
    Emoji getByCode(uint emojiCode) const;
    QStringList getCategories() const;

    bool codeIsEmoji(uint code) const;
    QString emojify(const QString &string) override;

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
