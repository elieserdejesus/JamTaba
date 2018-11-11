#ifndef _GUI_UTILS_
#define _GUI_UTILS_

#include <QString>
#include <QLayout>

namespace gui {

    QString capitalize(const QString &string);

    void setLayoutItemsVisibility(QLayout *layout, bool visible);

    QString sanitizeServerName(const QString &serverName);

    void clearLayout(QLayout *layout);

} // namespace

#endif
