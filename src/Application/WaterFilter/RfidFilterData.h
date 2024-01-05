/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDFILTERDATA_H
#define RFIDFILTERDATA_H

#include "RfidFilterUpdateRateData.h"

typedef struct
{
   RfidFilterUpdateRateData_t *rfidFilterUpdateRateData;
   uint16_t filterMonthInMinutes;
} RfidFilterData_t;

#endif
