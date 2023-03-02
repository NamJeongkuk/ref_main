/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include "NonVolatileDataSourceDefaultData.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

void NonVolatileDataSourceDefaultData_Zeros(void *data, size_t dataSize)
{
   memset(data, 0, dataSize);
}

void NonVolatileDataSourceDefaultData_BooleanTrue(void *data, size_t dataSize)
{
   memcpy(data, &constTrue, dataSize);
}

void NonVolatileDataSourceDefaultData_BooleanFalse(void *data, size_t dataSize)
{
   memcpy(data, &constFalse, dataSize);
}

void NonVolatileDataSourceDefaultData_Code(void *data, size_t dataSize)
{
   uint32_t code = 0xC0DECAFE;
   memcpy(data, &code, dataSize);
}

void NonVolatileDataSourceDefaultData_Int8Max(void *data, size_t dataSize)
{
   memset(data, INT8_MAX, dataSize);
}
void NonVolatileDataSourceDefaultData_DispenseSelection(void *data, size_t dataSize)
{
   DispenseSelection_t defaultDispenseSelection = DispenseSelection_ColdWater;
   memcpy(data, &defaultDispenseSelection, dataSize);
}
