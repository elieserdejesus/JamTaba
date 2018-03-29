#ifndef USERS_COLORS_POOL_H
#define USERS_COLORS_POOL_H

#include <QColor>
#include <QList>
#include <QString>
#include <QMap>

class UsersColorsPool
{

public:
    UsersColorsPool();
    QColor get(const QString &userName); // used to get a 'almost unique' color for each user
    void giveBack(const QString &userName); // give back the user color to the pool, this color will be reused

    void giveBackAllColors();

private:
    QList<QColor> availableColors;
    QMap<QString, QColor> pool;

    QList<QColor> createColors(int totalColors) const;

    static const int TOTAL_COLORS;
};

#endif
