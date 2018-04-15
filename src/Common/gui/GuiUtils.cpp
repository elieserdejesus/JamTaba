#include "GuiUtils.h"

#include <QWidget>
#include <QString>
#include <QDir>

QString gui::capitalize(const QString &string)
{
    return string.left(1).toUpper() + string.mid(1);
}

void gui::setLayoutItemsVisibility(QLayout *layout, bool visible)
{
    for (int i = 0; i < layout->count(); ++i) {
        auto item = layout->itemAt(i);
        if (item->layout()) {
            setLayoutItemsVisibility(item->layout(), visible);
        } else {
            auto widget = item->widget();
            if (widget)
                widget->setVisible(visible);
        }
    }
}

QString gui::sanitizeServerName(const QString &serverName)
{
    if (serverName.contains(" "))
        return serverName.split(" ").first();

    if (serverName.count(QChar('.')) == 1) // ninbot.com, ninjamer.com
        return serverName.split(".").first();

    return serverName;
}

