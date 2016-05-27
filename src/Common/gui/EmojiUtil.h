#ifndef EMOJIUTIL_H
#define EMOJIUTIL_H

#include <QRegExp>

namespace EmojiUtil
{
  /**
   * replace emoticon notation to HTML img tag.
   */
  static QString emojify(QString text)
  {
    static const QRegExp before(":([^\\:\\s]+):");
    static const QString after("<img title=\"\\1\" src=\":/emoji/\\1.png\">");

    return text.replace(before, after);
  }
}

#endif // EMOJIUTIL_H
