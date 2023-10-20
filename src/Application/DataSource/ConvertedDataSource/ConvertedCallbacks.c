/*!
 * @file
 * @brief Implements read / write callback functions for converted ERDs converted datasource
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "ConvertedCallbacks.h"
#include "SystemErds.h"
#include "I_DataSource.h"
#include "utils.h"
#include "SerialNumber.h"
#include "RfidTypes.h"

void ErdConvertedCallBack_WriteNotAvailable(void *context, const void *dataToWrite)
{
   IGNORE_ARG(context);
   IGNORE_ARG(dataToWrite);
}

void ErdConvertedCallBack_ReadRfidFilterUnitSerialNumber(void *context, void *dataToFill)
{
   I_DataSource_t *dataSource = context;

   SerialNumber_t unitSerialNumber;
   DataSource_Read(
      dataSource,
      Erd_SerialNumber,
      &unitSerialNumber);

   memcpy(dataToFill, &unitSerialNumber, sizeof(UnitSerialNumber_t));
}
