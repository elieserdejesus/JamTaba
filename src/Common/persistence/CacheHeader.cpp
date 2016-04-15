#include "CacheHeader.h"
#include <QDataStream>

const quint32 CacheHeader::SIGNATURE = 0x4a544232; // "JTB2"
const quint32 CacheHeader::SIZE = 12; // 3x quint32

CacheHeader::CacheHeader(quint32 revision)
    :revision(revision)
{
    if (revision > 0) { //creating a instance to write?
        size = CacheHeader::SIZE;
        signature = CacheHeader::SIGNATURE;
    }
}

QDataStream &operator >>(QDataStream &stream, CacheHeader &cacheHeader)
{
    stream >> cacheHeader.signature;
    stream >> cacheHeader.revision;
    stream >> cacheHeader.size;
    return stream;
}

QDataStream &operator <<(QDataStream &stream, const CacheHeader &cacheHeader)
{
    stream << cacheHeader.signature;
    stream << cacheHeader.revision;
    stream << cacheHeader.size;
    return stream;
}
