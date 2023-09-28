/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDFILTERUPDATERATEDATA_H
#define RFIDFILTERUPDATERATEDATA_H

#include <stdint.h>

typedef struct
{
   uint8_t doorOpenFilterReadFrequencyInSeconds;
   uint8_t doorJustClosedFilterReadFrequencyInSeconds;
   uint8_t doorClosedFilterUpdateTimeInMinutes;
   uint8_t doorClosedRfidErrorDetectedUpdateFrequencyInSeconds;
   uint8_t doorJustClosedFilterReadTimeoutInSeconds;
   uint8_t rfidFilterWriteTimeoutInMinutes;
} RfidFilterUpdateRateData_t;

#endif
