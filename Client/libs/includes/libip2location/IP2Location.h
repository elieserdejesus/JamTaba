/*
 * IP2Location C library is distributed under LGPL version 3
 * Copyright (c) 2013-2015 IP2Location.com. support at ip2location dot com 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not see <http://www.gnu.org/licenses/>.
 */
#ifndef HAVE_IP2LOCATION_H
#define HAVE_IP2LOCATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h> 
#if !defined(__APPLE__)
#include <stdlib.h> 
#endif

#ifdef WIN32
#define int16_t short
#define int32_t int
#define int64_t long long int
#endif

#ifndef WIN32
#include <stdint.h>
#else

#ifndef uint8_t
#define uint8_t unsigned char
#endif

#ifndef uint16_t
#define uint16_t short
#endif

#ifndef int32_t 
#define int32_t int
#endif

#ifndef int64_t
#define int64_t long long int
#endif

#ifndef uint32_t
#ifndef WIN32
#define uint32_t int
#else
#define uint32_t unsigned int
#endif
#endif
#endif

#include "IP2Loc_DBInterface.h"

#define API_VERSION   7.0.2


#define API_VERSION_MAJOR   7
#define API_VERSION_MINOR   0
#define API_VERSION_RELEASE 2
#define API_VERSION_NUMERIC (((API_VERSION_MAJOR * 100) + API_VERSION_MINOR) * 100 + API_VERSION_RELEASE)

#define MAX_IPV4_RANGE  4294967295U
#define MAX_IPV6_RANGE  "340282366920938463463374607431768211455"
#define IPV4 0
#define IPV6 1

#define  COUNTRYSHORT           0x00001
#define  COUNTRYLONG            0x00002
#define  REGION                 0x00004
#define  CITY                   0x00008
#define  ISP                    0x00010
#define  LATITUDE               0x00020
#define  LONGITUDE              0x00040
#define  DOMAIN                 0x00080
#define  ZIPCODE                0x00100
#define  TIMEZONE               0x00200
#define  NETSPEED               0x00400 
#define  IDDCODE                0x00800
#define  AREACODE               0x01000
#define  WEATHERSTATIONCODE     0x02000
#define  WEATHERSTATIONNAME     0x04000
#define  MCC                    0x08000
#define  MNC                    0x10000
#define  MOBILEBRAND            0x20000
#define  ELEVATION              0x40000
#define  USAGETYPE              0x80000

#define  ALL          COUNTRYSHORT | COUNTRYLONG | REGION | CITY | ISP | LATITUDE | LONGITUDE | DOMAIN | ZIPCODE | TIMEZONE | NETSPEED | IDDCODE | AREACODE | WEATHERSTATIONCODE | WEATHERSTATIONNAME | MCC | MNC | MOBILEBRAND | ELEVATION | USAGETYPE

#define  DEFAULT	     0x0001
#define  NO_EMPTY_STRING 0x0002
#define  NO_LEADING      0x0004
#define  NO_TRAILING     0x0008

#define INVALID_IPV6_ADDRESS "INVALID IPV6 ADDRESS"
#define INVALID_IPV4_ADDRESS "INVALID IPV4 ADDRESS"
#define  NOT_SUPPORTED "This parameter is unavailable for selected data file. Please upgrade the data file."


typedef struct
{
	FILE *filehandle;
	uint8_t databasetype;
	uint8_t databasecolumn;
	uint8_t databaseday;
	uint8_t databasemonth;
	uint8_t databaseyear;
	uint32_t databasecount;
	uint32_t databaseaddr;
	uint32_t ipversion;
	uint32_t ipv4databasecount;
	uint32_t ipv4databaseaddr;
	uint32_t ipv6databasecount;
	uint32_t ipv6databaseaddr;
} IP2Location;

typedef struct 
{
	char *country_short;
	char *country_long;
	char *region;
	char *city;
	char *isp;
	float latitude;
	float longitude;
	char *domain;
	char *zipcode;
	char *timezone;
	char *netspeed;
	char *iddcode;
	char *areacode;
	char *weatherstationcode;
	char *weatherstationname;
	char *mcc;
	char *mnc;
	char *mobilebrand;
	float elevation;
	char *usagetype;
} IP2LocationRecord;

/*##################
# Public Functions
##################*/
IP2Location *IP2Location_open(char *db);
int IP2Location_open_mem(IP2Location *loc, enum IP2Location_mem_type);
uint32_t IP2Location_close(IP2Location *loc);
IP2LocationRecord *IP2Location_get_country_short(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_country_long(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_region(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_city (IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_isp(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_latitude(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_longitude(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_domain(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_zipcode(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_timezone(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_netspeed(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_iddcode(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_areacode(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_weatherstationcode(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_weatherstationname(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_mcc(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_mnc(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_mobilebrand(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_elevation(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_usagetype(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_all(IP2Location *loc, char *ip);
void IP2Location_free_record(IP2LocationRecord *record);
void IP2Location_delete_shm();
unsigned long int IP2Location_api_version_num(void);
char *IP2Location_api_version_string(void);

#ifdef __cplusplus
}
#endif

#endif
