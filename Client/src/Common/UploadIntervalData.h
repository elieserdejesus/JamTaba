#ifndef UPLOAD_INTERVAL_DATA_H
#define UPLOAD_INTERVAL_DATA_H

#include <QByteArray>

class UploadIntervalData
{
public:
    UploadIntervalData();

    inline QByteArray getGUID() const
    {
        return GUID;
    }

    void appendData(QByteArray encodedData);

    inline int getTotalBytes() const
    {
        return dataToUpload.size();
    }

    inline QByteArray getStoredBytes() const
    {
        return dataToUpload;
    }

    inline void clear()
    {
        dataToUpload.clear();
    }

private:
    static QByteArray newGUID();
    const QByteArray GUID;
    QByteArray dataToUpload;
};

#endif
