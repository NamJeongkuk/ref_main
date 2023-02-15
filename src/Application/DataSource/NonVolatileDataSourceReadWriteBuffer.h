/*!
 * @file
 * @brief Nonvolatile Data Source Read / write buffer
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NONVOLATILEDATASOURCEREADWRITEBUFFER_H
#define NONVOLATILEDATASOURCEREADWRITEBUFFER_H

#include "SystemErds.h"

#define EXPAND_AS_NON_VOLATILE_DATA_SOURCE_READ_WRITE_BUFFER_MEMBERS(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)                                                                                                         \
   (uint8_t CONCAT(erd, Name)[sizeof(DataType) + sizeof(Crc16_t)];)

typedef struct
{
   union
   {
      ERD_TABLE(EXPAND_AS_NON_VOLATILE_DATA_SOURCE_READ_WRITE_BUFFER_MEMBERS)
      uint64_t alignment;
   } data;
} NonVolatileDataSourceReadWriteBuffer_t;

#endif
