#ifndef UPLOAD_INTERVAL_DATA_H
#define UPLOAD_INTERVAL_DATA_H

#include <QByteArray>

class UploadIntervalData
{

public:
    UploadIntervalData();
    ~UploadIntervalData();

    inline QByteArray getGUID() const
    {
        return GUID;
    }

    inline bool isEmpty() const
    {
        return dataToUpload.isEmpty();
    }

    void appendData(const QByteArray &encodedData);

    inline int getTotalBytes() const
    {
        return dataToUpload.size();
    }

    inline QByteArray getData() const
    {
        return dataToUpload;
    }

    inline void clear()
    {
        dataToUpload.clear();
    }

private:
    static QByteArray newGUID();
    QByteArray GUID;
    QByteArray dataToUpload;

};

#endif
