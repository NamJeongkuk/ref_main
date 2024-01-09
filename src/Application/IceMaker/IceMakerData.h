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
#include "NonHarvestFillTubeHeaterData.h"
#include "IceMakerFillBlockerData.h"

typedef struct
{
   FreezerIceRateData_t *freezerIceRateData;
   AluminumMoldIceMakerData_t *aluminumMoldIceMakerData;
   TwistTrayIceMakerData_t *twistTrayIceMakerData;
   NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData;
   IceMakerFillBlockerData_t *iceMakerFillBlockerData;
} IceMakerData_t;

#endif
