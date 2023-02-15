/*!
 * @file
 * @brief Input Group to read default data non-volatile data source
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INPUTGROUP_NONVOLATILEDATASOURCEDEFAULTDATA_H
#define INPUTGROUP_NONVOLATILEDATASOURCEDEFAULTDATA_H

#include "I_InputGroup.h"
#include "AsyncDataSource_Eeprom.h"

typedef struct
{
   I_InputGroup_t interface;
   struct
   {
      const AsyncDataSource_EepromErdInfo_t *erdInfoTable;
      uint16_t erdCount;
   } _private;
} InputGroup_NonVolatileDataSourceDefaultData_t;

/*!
 * @param instance
 * @param erdTable The table of flash block group erd information
 * @param erdCount The number of entries in the table
 */
void InputGroup_NonVolatileDataSourceDefaultData_Init(
   InputGroup_NonVolatileDataSourceDefaultData_t *instance,
   const AsyncDataSource_EepromErdInfo_t *erdInfoTable,
   const uint16_t erdCount);

#endif
