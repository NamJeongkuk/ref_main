/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FAULTID_H
#define FAULTID_H

#include "SystemErds.h"
#include <stdint.h>

#define EXPAND_AS_FAULT_ID_ENUM(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_FAULT_, StorageType)                                                                      \
   (FaultId_##Name = FaultId COMMA)

enum
{
   FaultId_None = 0,
   ERD_TABLE(EXPAND_AS_FAULT_ID_ENUM)
};
typedef uint16_t FaultId_t;

#endif
