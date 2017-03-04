#ifndef _GUI_UTILS_
#define _GUI_UTILS_

#include <QString>
#include <QLayout>

namespace Gui {

QString capitalize(const QString &string);

void setLayoutItemsVisibility(QLayout *layout, bool visible);

}//namespace Gui

#endif
