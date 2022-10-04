/*!
 * @file
 * @brief The module subscribes compressor controller speed which is decided from compressor controller.
 * And, it controls compressor relay and update compressor frequency based on compressor speed, cooling mode and
 * enumerated freezer set point.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSPEEDDRIVER_H
#define COMPRESSORSPEEDDRIVER_H

#include "I_DataModel.h"
#include "CompressorData.h"

typedef struct
{
   Erd_t compressorRelayErd; // bool
   Erd_t compressorFrequencyErd; // uint8_t
   Erd_t coolingModeErd; // CoolingMode_t
   Erd_t freezerSetpointZoneErd; // SetpointZone_t
   Erd_t compressorControllerSpeedErd; // CompressorSpeed_t
} CompressorSpeedDriverConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t compressorSpeedSubscription;
      const CompressorData_t *compressorParametricData;
      const CompressorSpeedDriverConfig_t *config;
   } _private;
} CompressorSpeedDriver_t;

void CompressorSpeedDriver_Init(
   CompressorSpeedDriver_t *instance,
   I_DataModel_t *dataModel,
   const CompressorSpeedDriverConfig_t *config);

#endif
