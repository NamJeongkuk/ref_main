/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETFANSTARTUPDATA_H
#define ICECABINETFANSTARTUPDATA_H

#include "FanSpeed.h"
#include <stdint.h>

typedef struct
{
   FanSpeed_t startupSpeed;
   uint8_t startupTimeInSeconds;
} IceCabinetFanStartupData_t;

#endif
