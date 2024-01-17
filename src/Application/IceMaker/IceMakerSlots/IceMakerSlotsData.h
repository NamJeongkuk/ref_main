/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERSLOTSDATA_H
#define ICEMAKERSLOTSDATA_H

#include <stdint.h>
#include "IceMakerType.h"
#include "IceMakerLocation.h"
#include "TwistTrayIceMakerData.h"
#include "AluminumMoldIceMakerData.h"
#include "FreezerIceRateData.h"
#include "NonHarvestFillTubeHeaterData.h"

typedef struct
{
   bool enable;
   IceMakerType_t type;
   IceMakerLocation_t location;
   union
   {
      TwistTrayIceMakerData_t *twistTrayData;
      AluminumMoldIceMakerData_t *aluminumMoldData;
   };
   FreezerIceRateData_t *freezerIceRateData;
   NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData;
} IceMakerSlotData_t;

typedef struct
{
   IceMakerSlotData_t *slot0Data;
   IceMakerSlotData_t *slot1Data;
   IceMakerSlotData_t *slot2Data;
} IceMakerSlotsData_t;

#endif
