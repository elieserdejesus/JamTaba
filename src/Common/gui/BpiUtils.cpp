#include "BpiUtils.h"

#include <QtGlobal>

QList<uint> bpiUtils::getBpiDividers(uint targetBpi)
{
    QList<uint> foundedDividers;
    uint divider = 2;
    while (divider <= targetBpi / 2) {
        if (targetBpi % divider == 0)
            foundedDividers.append(divider);
        divider++;
    }

    if (foundedDividers.isEmpty())
        foundedDividers.append(targetBpi); // using bpi as default

    return foundedDividers;
}
