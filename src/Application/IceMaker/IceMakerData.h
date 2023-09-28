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
#include "TwistTrayIceMakerData.h"
#include "AugerMotorData.h"
#include "NonHarvestFillTubeHeaterData.h"
#include "IceMakerFillBlocker.h"

typedef struct
{
   FreezerIceRateData_t *freezerIceRateData;
   AluminumMoldIceMakerData_t *aluminumMoldIceMakerData;
   TwistTrayIceMakerData_t *twistTrayIceMakerData;
   AugerMotorData_t *augerMotorData;
   NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData;
   IceMakerFillBlocker_t *iceMakerFillBlocker;
} IceMakerData_t;

#endif
