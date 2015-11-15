#include "BpiUtils.h"

BpiUtils::BpiUtils()
{

}

QList<int> BpiUtils::getBpiDividers(int targetBpi){
    QList<int> foundedDividers;
    int divider = 2;
    while (divider <= targetBpi / 2) {
        if (targetBpi % divider == 0) {
            foundedDividers.append(divider);
        }
        divider++;
    }

    if (foundedDividers.isEmpty()) {
        foundedDividers.append(targetBpi);//using bpi as default
    }
    return foundedDividers;
}
