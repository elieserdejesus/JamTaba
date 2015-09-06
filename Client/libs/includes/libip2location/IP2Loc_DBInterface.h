/*
 * IP2Location C library is distributed under LGPL version 3
 * Copyright (c) 2013 IP2Location.com. support at ip2location dot com 
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
 *
 */ 

#ifndef HAVE_IP2LOC_DBINTERFACE_H
#define HAVE_IP2LOC_DBINTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif


enum IP2Location_mem_type
{
	IP2LOCATION_FILE_IO,
	IP2LOCATION_CACHE_MEMORY,
	IP2LOCATION_SHARED_MEMORY
};

struct in6_addr_local
{
	union {
		uint8_t addr8[16];
		uint8_t addr16[8];
	}u;
};


/*All below function are private function IP2Location library*/
struct in6_addr_local IP2Location_readIPv6Address(FILE *handle, uint32_t position);
uint32_t IP2Location_read32(FILE *handle, uint32_t position);
uint8_t IP2Location_read8(FILE *handle, uint32_t position);
char *IP2Location_readStr(FILE *handle, uint32_t position);
float IP2Location_readFloat(FILE *handle, uint32_t position);
int32_t IP2Location_DB_set_file_io();
int32_t IP2Location_DB_set_memory_cache(FILE *filehandle);
int32_t IP2Location_DB_set_shared_memory(FILE *filehandle);
int32_t IP2Location_DB_close(FILE *filehandle);
void IP2Location_DB_del_shm();

#ifdef __cplusplus
}
#endif
#endif
