/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERFILLMONITORDATA_H
#define ICEMAKERFILLMONITORDATA_H

#include <stdint.h>

typedef struct
{
   uint16_t timedIceMakerFillInSecondsx10;
   uint16_t preciseIceMakerFillMaxTimeInSecondsx10;
   uint16_t timedIceMakerFillOuncesInOzx100;
} IceMakerFillMonitorData_t;

#endif
