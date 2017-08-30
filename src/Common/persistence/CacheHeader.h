#ifndef _CACHE_HEADER_
#define _CACHE_HEADER_

#include <QtGlobal>

class CacheHeader
{

    friend QDataStream &operator >>(QDataStream &stream, CacheHeader &cacheHeader);
    friend QDataStream &operator <<(QDataStream &stream, const CacheHeader &cacheHeader);

public:
    CacheHeader(quint32 revision = 0);

    bool isValid(quint32 expectedRevision) const;

private:

    quint32 signature;
    quint32 revision;
    quint32 size;

    static const quint32 SIGNATURE;
    static const quint32 SIZE;
};

#endif
