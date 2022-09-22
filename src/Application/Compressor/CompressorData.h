/*!
 * @file
 * @brief compressor parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORDATA_H
#define COMPRESSORDATA_H

#include "CompressorSpeeds.h"
#include "CompressorTimes.h"

typedef struct
{
   bool compressorIsSingleSpeed;
   int16_t lowAmbientTriggerTemperatureInDegFx100;
   bool variableSpeedCompressorRelayIsEnabled;
   CompressorTimes_t compressorTimes;
   CompressorSpeeds_t compressorSpeeds;
} CompressorData_t;

#endif
