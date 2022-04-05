/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTDATA_H
#define DEFROSTDATA_H

#include "TemperatureDegFx100.h"
#include "ValvePosition.h"

typedef struct
{
   uint16_t fzDoorIncrementFactorInSecondsPerSecond;
   uint16_t ffDoorIncrementFactorInSecondsPerSecond;
   uint16_t fzAbnormalRunTimeInMinutes;
   uint16_t maxTimeBetweenDefrostsInMinutes;
   TemperatureDegFx100_t dmFzDefrostTemperatureInDegFx100;
   TemperatureDegFx100_t prechillFzSetpointInDegFx100;
   TemperatureDegFx100_t prechillFfSetpointInDegFx100;
   TemperatureDegFx100_t prechillCcSetpointInDegFx100;
   TemperatureDegFx100_t prechillFzEvapExitTemperatureInDegFx100;
   TemperatureDegFx100_t prechillCcEvapExitTemperatureInDegFx100;
   uint8_t maxPrechillTimeInMinutes;
   uint8_t defrostDoorHoldoffTimeForFfAndFzInMinutes;
   uint8_t defrostDoorHoldoffTimeForFfOnlyInMinutes;
   uint16_t defrostMaxHoldoffTimeInMinutes;
   uint16_t maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds;
   TemperatureDegFx100_t ffFanDefrostFfEvapExitTemperatureInDegFx100;
   uint16_t ffFanDefrostFfFanMaxOnTimeInMinutes;
   TemperatureDegFx100_t ccFanDefrostCcEvapExitTemperatureInDegFx100;
   uint16_t ccFanDefrostCcFanMaxOnTimeInMinutes;
   uint8_t fzDefrostHeaterMaxOnTimeInMinutes;
   uint8_t fzAbnormalDefrostHeaterMaxOnTimeInMinutes;
   TemperatureDegFx100_t fzDefrostTerminationTemperatureInDegFx100;
   TemperatureDegFx100_t ffDefrostTerminationTemperatureInDegFx100;
   TemperatureDegFx100_t ccDefrostTerminationTemperatureInDegFx100;
   uint8_t ffDefrostHeaterMaxOnTimeInMinutes;
   uint8_t ffAbnormalDefrostHeaterMaxOnTimeInMinutes;
   uint8_t ccDefrostHeaterMaxOnTimeInMinutes;
   uint8_t ccAsFfAbnormalDefrostHeaterMaxOnTimeInMinutes;
   uint8_t ccAsFzAbnormalDefrostHeaterMaxOnTimeInMinutes;
   uint8_t defrostDwellTimeInMinutes;
   TemperatureDegFx100_t ffAndFzPostDwellFzExitTemperatureInDegFx100;
   uint8_t ffAndFzPostDwellFzExitTimeInMinutes;
   ValvePosition_t dwellThreeWayValvePosition;
   ValvePosition_t postDwellThreeWayValvePositionForFfAndFz;
   TemperatureDegFx100_t ffPostDefrostPullDownExitTemperatureInDegFx100;
   TemperatureDegFx100_t fzPostDefrostPullDownExitTemperatureInDegFx100;
   uint8_t numberOfFfDefrostsBeforeFzDefrost;
   uint8_t numberOfFfDefrostsBeforeAbnormalFzDefrost;
   uint8_t doorHoldoffTimeForFfAndFzInMinutes;
   uint8_t ffOnlyPostDwellExitTimeInMinutes;
   TemperatureDegFx100_t dsmFzSetpointTemperatureInDegFx100;
   uint8_t defrostPeriodicTimeoutInSeconds;
   ValvePosition_t threeWayValvePositionToExitIdle;
} DefrostData_t;

#endif
