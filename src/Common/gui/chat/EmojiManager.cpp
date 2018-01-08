#include "EmojiManager.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QJsonObject>
#include <QFile>
#include <QDateTime>
#include <QStandardItemModel>

const uint EmojiManager::ICONS_SIZE = 24;

const QStringList EmojiManager::categories = QStringList()
        << "Recent"
        << "Smileys & People"
        << "Music"
        << "Animals & Nature"
        << "Activities"
        << "Food & Drink"
        << "Travel & Places"
        << "Objects"
        << "Symbols";

const QMap<QString, QString> EmojiManager::combinationsMap = QMap<QString, QString>({
    {":)", "1F603"},
    {":-)", "1F603"},
    {":(", "1F61E"},
    {":-(", "1F61E"},
    {";)", "1F609"},
    {";-)", "1F609"},
    {":-o)", "1F632"},
    {":-O)", "1F632"},
    {":/)", "1F615"},
    {":-/)", "1F615"},
    {":D", "1F601"},
    {":-D)", "1F601"},
    {":@", "1F620"},
    {":-@)", "1F620"},
    {"(y)", "1F44D"},
    {"(n)", "1F44E"}
});


Emoji::Emoji(const QString &name, const QString category, uint sortOrder, const QString &unifiedCode) :
    name(name),
    category(category),
    sortOrder(sortOrder),
    unifiedCode(unifiedCode)
{
    uint code = !unifiedCode.contains("-") ? unifiedCode.toInt(0, 16) : 0;
    isMusical = (code >= 0x1f398 && code <= 0x1f39d) ||
            (code >= 0x1f3b5 && code <= 0x1f3bc) ||
            (code >= 0x2669 && code <= 0x266f) ||
            (code == 0x1f3a4 || code == 0x1f3a7 || code == 0x1F50A);

}

bool Emoji::operator ==(const Emoji &other)
{
    return unifiedCode == other.unifiedCode;
}

QString EmojiManager::getEmojiIconUrl(const Emoji &emoji) const
{
    return getEmojiIconUrl(emoji.name);
}

QString EmojiManager::getEmojiIconUrl(const QString &emojiName) const
{
    return QString("%1/%2.png")
            .arg(iconsPath)
            .arg(emojiName);
}

QAbstractItemModel *EmojiManager::getDataModel(int completeRole)
{
    QStandardItemModel *model = new QStandardItemModel();
    model->setColumnCount(1);
    model->setRowCount(generalMap.count());

    int row = 0;
    for (const Emoji &emoji : generalMap.values()) {
        QString prettyName = QString(emoji.name).replace("_", " ");
        QStandardItem* item = new QStandardItem(prettyName);
        item->setIcon(QPixmap(getEmojiIconUrl(emoji)));
        item->setToolTip(prettyName);
        item->setData(emoji.unifiedCode, completeRole);
        model->setItem(row, 0, item);
        row++;
    }

    return model;
}

bool emojiLessThan(const Emoji &e1, const Emoji &e2)
{
    return e1.sortOrder < e2.sortOrder;
}

 bool EmojiManager::codeIsEmoji(uint code)
 {
     return generalMap.contains(code);
 }

 EmojiManager::EmojiManager(const QString &emojisJsonPath, const QString &emojiIconsPath) :
     iconsPath(emojiIconsPath)
 {
     loadData(emojisJsonPath);
 }

QString EmojiManager::emojify(const QString &string)
{
    QString replacedString(string);

    for (const QString &combination : combinationsMap.keys())
        replacedString.replace(combination, emojiCodeToUtf8(combinationsMap[combination]));

    QVector<uint> codes = replacedString.toUcs4();
    QString newString;
    for (uint code : codes) {
        if (!EmojiManager::codeIsEmoji(code)) {
            newString.append(QString::fromUcs4(&code, 1));
        }
        else{
            Emoji emoji = getByCode(code);
            newString.append(QString("<img src=%1>").arg(getEmojiIconUrl(emoji)));
        }
    }
     return newString;
}

QStringList EmojiManager::getCategories() const
{
    return EmojiManager::categories;
}

QList<Emoji> EmojiManager::getByCategory(const QString &category)
{
    return categorizedMap[category];
}

void EmojiManager::loadData(const QString &jsonPath)
{

    QStringList skipCategories;
    skipCategories << "Flags";

    QFile json(jsonPath);
    if (!json.open(QIODevice::ReadOnly)) {
        qCritical() << "Error loading emoji.json" << json.errorString();
        return;
    }

    auto doc = QJsonDocument::fromJson(json.readAll());

    QJsonArray root = doc.array();
    for (int i = 0; i < root.count(); ++i) {

        QJsonObject emojiData = root.at(i).toObject();

        QString name = emojiData.value("short_name").toString();

        // avoid emoji if the image is not founded in resources
        if (!QFile(getEmojiIconUrl(name)).exists())
            continue;

        QString category = emojiData.value("category").toString();
        if (skipCategories.contains(category))
            continue;

        QString unifiedCode = emojiData.value("unified").toString();
        uint code = unifiedCode.toInt(0, 16);

        if (code >= 0x1f6d0) // high value codes are not working
            continue;

        uint sortOrder = emojiData.value("sort_order").toInt();

        Emoji emoji(name, category, sortOrder, unifiedCode);

        if (!category.isEmpty()) {
            categorizedMap[category] << emoji;
            if (emoji.isMusical)
                categorizedMap["Music"] << emoji;
        }

        generalMap.insert(code, emoji);
    }

    //sort each loaded category
    for (auto key : categorizedMap.keys()) {
        auto &emojis = categorizedMap[key];
        qSort(emojis.begin(), emojis.end(), emojiLessThan);
    }
}

Emoji EmojiManager::getByCode(uint emojiCode)
{
    return generalMap[emojiCode];
}

QString EmojiManager::emojiCodeToUtf8(const QString &emojiCode)
{
    static const int MAX_CODES = 4;  // max of 4 codes

    uint codePoints[MAX_CODES];

    QStringList codes = emojiCode.split("-");
    for (int i = 0; i < qMin(codes.size(), MAX_CODES); ++i) {
        codePoints[i] = codes.at(i).toInt(0, 16);
    }

    return QString::fromUcs4(codePoints, static_cast<int>(codes.size()));
}

void EmojiManager::addRecent(const QString &emojiCode)
{
    Emoji emoji = getByCode(emojiCode.toInt(0, 16));

    categorizedMap["Recent"].removeAll(emoji);
    categorizedMap["Recent"].push_front(emoji);

    recents << emojiCode;
}

