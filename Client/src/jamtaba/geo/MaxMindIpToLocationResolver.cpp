#include "MaxMindIpToLocationResolver.h"

#include <QDebug>

using namespace Geo;

MaxMindIpToLocationResolver::MaxMindIpToLocationResolver(QString databasePath){
    int status = MMDB_open(databasePath.toStdString().c_str(), MMDB_MODE_MMAP, &this->mmdb_s);
    if (MMDB_SUCCESS != status) {
        qWarning() << "can't open " << databasePath;
        if (MMDB_IO_ERROR == status) {
            qWarning() << strerror(errno);
        }
    }
}

MaxMindIpToLocationResolver::~MaxMindIpToLocationResolver(){
    MMDB_close(&mmdb_s);
}

Location MaxMindIpToLocationResolver::resolve(QString ip) {
    if(ip.isEmpty()){
        return Location();//empty location
    }
    int gai_error, mmdb_error;
    MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb_s, ip.toStdString().c_str(), &gai_error, &mmdb_error);
    if (0 != gai_error) {
        qWarning() << "Error from getaddrinfo for " << ip << " - " << gai_strerror(gai_error);
    }
    if (MMDB_SUCCESS != mmdb_error) {
        qWarning() << "Got an error from libmaxminddb: " << MMDB_strerror(mmdb_error);
    }
    if (result.found_entry) {
        MMDB_entry_data_s data;
        MMDB_get_value(&result.entry, &data, "country", "names", "en", NULL);
        QString countryName = QString(data.utf8_string).left(data.data_size);

        MMDB_get_value(&result.entry, &data, "country", "iso_code", NULL);
        QString countryCode = QString(data.utf8_string).left(data.data_size);
        return Location(countryName, countryCode);
    }
    return Location();//return an empty location
}



