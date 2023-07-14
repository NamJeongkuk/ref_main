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
#include "IceMakerTypeInformation.h"
#include "IceMakerFillMonitorData.h"
#include "HarvestCountCalculatorData.h"

typedef struct
{
   const IceMakerFillMonitorData_t *iceMakerFillMonitorData;
} TwistTrayIceMakerFillData_t;

typedef struct
{
   const HarvestCountCalculatorData_t *harvestCountCalculatorData;
   TemperatureDegFx100_t maximumHarvestTemperatureInDegFx100;
} TwistTrayIceMakerFreezeData_t;

typedef struct
{
   uint8_t fullBucketWaitPeriodMinutes;
   uint8_t fullBucketDetectionPeriodSecX10;
   uint8_t initialHomingTwistPeriodSecX10;
   uint8_t homeLandingDelayPeriodSecX10;
   uint8_t motorErrorRetryInitializeMinutes;
   uint8_t longMotorErrorTimeoutPeriodSec;
   uint8_t shortMotorErrorTimeoutPeriodSec;
   uint8_t delayToHarvestAfterDoorClosesSeconds;
   uint8_t fullBucketDispenseCheckTimeInSeconds;
   uint16_t fillTubeHeaterOnTimeInSeconds;
   uint8_t fillTubeHeaterDutyCyclePercentage;
} TwistTrayIceMakerHarvestData_t;

typedef struct
{
   IceMakerTypeInformation_t typeInformation;
   TwistTrayIceMakerFillData_t fillData;
   TwistTrayIceMakerFreezeData_t freezeData;
   TwistTrayIceMakerHarvestData_t harvestData;
} TwistTrayIceMakerData_t;

#endif
