#include "UploadIntervalData.h"
#include <QUuid>

UploadIntervalData::UploadIntervalData() :
    GUID(newGUID())
{
}

UploadIntervalData::~UploadIntervalData()
{
    //
}

void UploadIntervalData::appendData(const QByteArray &encodedData)
{
    dataToUpload.append(encodedData);
}

QByteArray UploadIntervalData::newGUID()
{
    QUuid uuid = QUuid::createUuid();
    return uuid.toRfc4122();
}
