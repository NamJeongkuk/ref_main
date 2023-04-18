/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERDATA_H
#define TWISTTRAYICEMAKERDATA_H

#include <stdint.h>
#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t maximumHarvestTemperatureInDegFx100;
   TemperatureDegFx100_t startIntegrationTemperatureInDegFx100;
   uint32_t targetFreezeIntegrationSum;
   uint8_t minimumFreezeTimeMinutes;
} TwistTrayIceMakerFreezeData_t;

typedef struct
{
   uint8_t fullBucketWaitPeriodMinutes;
   uint8_t fullBucketDetectionPeriodSecX10;
   uint8_t initialHomingTwistPeriodSecX10;
   uint8_t homeLandingDelayPeriodSecX10;
   uint8_t longMotorErrorTimeoutPeriodSec;
   uint8_t shortMotorErrorTimeoutPeriodSec;
   uint8_t delayToHarvestAfterDoorClosesSeconds;
   uint8_t fullBucketDispenseCheckTimeInSeconds;
} TwistTrayIceMakerHarvestData_t;

typedef struct
{
   uint8_t waterFillTimeSecX10;
} TwistTrayIceMakerFillData_t;

typedef struct
{
   TwistTrayIceMakerFreezeData_t freezeData;
   TwistTrayIceMakerHarvestData_t harvestData;
   TwistTrayIceMakerFillData_t fillData;
} TwistTrayIceMakerData_t;

#endif
