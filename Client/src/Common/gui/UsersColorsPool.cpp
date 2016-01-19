#include "UsersColorsPool.h"
#include <QtGlobal>
#include <algorithm>
#include <QDateTime>

const int UsersColorsPool::TOTAL_COLORS = 8;

UsersColorsPool::UsersColorsPool() :
    availableColors(createColors(TOTAL_COLORS))
{
    qsrand(0);
}

QColor UsersColorsPool::get(const QString &userName)
{
    if (pool.contains(userName)) // user has a color
        return pool[userName];

    if (availableColors.isEmpty()) { // no more colors, returning a duplicated random color
        QList<QColor> colors = pool.values();
        return pool.values().at(qrand() % colors.size());
    }

    pool.insert(userName, availableColors.takeFirst());
    return pool[userName];
}

void UsersColorsPool::giveBack(const QString &userName)
{
    if (pool.contains(userName)) {
        availableColors.append(pool[userName]);
        pool.remove(userName);
    }
}

QList<QColor> UsersColorsPool::createColors(int totalColors) const
{
    QList<QColor> colors;
    for (int x = 0; x < totalColors; ++x) {
        QColor color;
        qreal h = (qreal)x/totalColors;
        color.setHsvF(h, 0.25, 0.95);
        colors.append(color);
    }
    srand(QDateTime::currentMSecsSinceEpoch());
    std::random_shuffle(colors.begin(), colors.end());
    return colors;
}
