#include "GuiUtils.h"
#include <QWidget>

QString Gui::capitalize(const QString &string)
{
    return string.left(1).toUpper() + string.mid(1);
}

void Gui::setLayoutItemsVisibility(QLayout *layout, bool visible)
{
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem *item = layout->itemAt(i);
        if (item->layout()) {
            setLayoutItemsVisibility(item->layout(), visible);
        } else {
            QWidget *widget = item->widget();
            if (widget)
                widget->setVisible(visible);
        }
    }
}
