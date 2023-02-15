/*!
 * Initialize a AsyncDataSource_Eeprom_TestDouble
 * @param instance The AsyncDataSource_Eeprom_TestDouble instance.
 * Copyright GE Appliances - Confidential - All rights reserved.
 */
#ifndef ASYNCDATASOURCE_EEPROM_TESTDOUBLE_H
#define ASYNCDATASOURCE_EEPROM_TESTDOUBLE_H

extern "C"
{
#include "AsyncDataSource_Eeprom.h"
#include "Crc16Calculator_Table.h"
#include "I_Crc16Calculator.h"
#include "I_Eeprom.h"
#include "ConstArrayMap_BinarySearch.h"
#include "InputGroup_NonVolatileDataSourceDefaultData.h"
}
#include "SystemErds.h"
#include "CrcSeed.h"
#include "AsyncDataSource_Eeprom.h"
#include "utils.h"
#include "NonVolatileDataSourceReadWriteBuffer.h"
#include "Timer.h"
#include "Eeprom_Model.h"

enum
{
   Size8K = 8192
};

typedef struct
{
   I_AsyncDataSource_t asyncDataSource;

   struct
   {
      uint8_t eeprom[Size8K];
      Eeprom_Model_t eepromModel;
      ConstArrayMap_BinarySearch_t asyncMap;
      NonVolatileDataSourceReadWriteBuffer_t asyncReadWriteBuffer;
      InputGroup_NonVolatileDataSourceDefaultData_t defaultDataInputGroup;
      AsyncDataSource_Eeprom_t asyncDataSourceEeprom;
   } _private;
} AsyncDataSource_Eeprom_TestDouble_t;

typedef AsyncDataSource_Eeprom_TestDouble_t Instance_t;
/*!
 * @param instance
 * @param timerModule
 * @param crc16Calculator
 */
void AsyncDataSource_Eeprom_TestDouble_Init(
   Instance_t *instance,
   TimerModule_t *timerModule,
   I_Crc16Calculator_t *crc16Calculator);

/*!
 * @param instance
 */
I_AsyncDataSource_t *AsyncDataSource_Eeprom_TestDouble_GetAsyncDataSource(Instance_t *instance);

#endif
