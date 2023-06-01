/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERDATA_H
#define ICEMAKERDATA_H

#include "AluminumMoldIceMakerData.h"
#include "FreezerIceRateData.h"
#include "IceMakerFillMonitorData.h"
#include "TwistTrayIceMakerData.h"
#include "AugerMotorData.h"

typedef struct
{
   FreezerIceRateData_t *freezerIceRateData;
   AluminumMoldIceMakerData_t *aluminumMoldIceMakerData;
   IceMakerFillMonitorData_t *iceMakerFillMonitorData;
   TwistTrayIceMakerData_t *twistTrayIceMakerData;
   IceMakerFillMonitorData_t *twistIceMakerFillMonitorData;
   AugerMotorData_t *augerMotorData;
} IceMakerData_t;

#endif
