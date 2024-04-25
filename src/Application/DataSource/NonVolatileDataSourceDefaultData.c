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

void NonVolatileDataSourceDefaultData_DefaultPersonality(void *data, size_t dataSize)
{
   AppliancePersonality_t defaultPersonality = 0;
   memcpy(data, &defaultPersonality, dataSize);
}

void NonVolatileDataSourceDefaultData_UnlockedControlLockStatus(void *data, size_t dataSize)
{
   ControlLockStatus_t defaultControlLockStatus = ControlLockStatus_Unlocked;
   memcpy(data, &defaultControlLockStatus, dataSize);
}

void NonVolatileDataSourceDefaultData_Code(void *data, size_t dataSize)
{
   uint32_t code = 0xC0DECAFE;
   memcpy(data, &code, dataSize);
}

void NonVolatileDataSourceDefaultData_UInt8Max(void *data, size_t dataSize)
{
   memset(data, UINT8_MAX, dataSize);
}

void NonVolatileDataSourceDefaultData_Int8Max(void *data, size_t dataSize)
{
   memset(data, INT8_MAX, dataSize);
}

void NonVolatileDataSourceDefaultData_Int16Max(void *data, size_t dataSize)
{
   int16_t temp = INT16_MAX;
   memcpy(data, &temp, dataSize);
}

void NonVolatileDataSourceDefaultData_DispenseSelection(void *data, size_t dataSize)
{
   DispenseSelection_t defaultDispenseSelection = DispenseSelection_ColdWater;
   memcpy(data, &defaultDispenseSelection, dataSize);
}

void NonVolatileDataSourceDefaultData_FullDefrost(void *data, size_t dataSize)
{
   DefrostType_t defaultDefrostType = DefrostType_Full;
   memcpy(data, &defaultDefrostType, dataSize);
}

void NonVolatileDataSourceDefaultData_MaxLightBrightness(void *data, size_t dataSize)
{
   PercentageDutyCycle_t maxLightBrightness = PercentageDutyCycle_Max;
   memcpy(data, &maxLightBrightness, dataSize);
}

void NonVolatileDataSoundDefaultData_LowSoundLevel(void *data, size_t dataSize)
{
   uint8_t soundLevel = 1;
   memcpy(data, &soundLevel, dataSize);
}
