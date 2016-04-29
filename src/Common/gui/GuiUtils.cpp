#include "GuiUtils.h"

QString Gui::capitalize(const QString &string)
{
    return string.left(1).toUpper() + string.mid(1);
}
