#ifndef BPIUTILS_H
#define BPIUTILS_H

#include <QList>

class BpiUtils
{
public:
    BpiUtils();

    static QList<int> getBpiDividers(int targetBpi);
};

#endif // BPIUTILS_H
