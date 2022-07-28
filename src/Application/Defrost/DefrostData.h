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
#include "DamperPosition.h"
#include "FanSpeed.h"

typedef struct
{
   uint8_t defrostPeriodicTimeoutInSeconds;
   uint16_t freezerDoorIncrementFactorInSecondsPerSecond;
   uint16_t freshFoodDoorIncrementFactorInSecondsPerSecond;
   uint16_t minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes;
   uint16_t maxTimeBetweenDefrostsInMinutes;
   uint8_t numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   uint8_t numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost;
   uint8_t maxPrechillPrepTimeInMinutes;
   uint8_t maxPrechillTimeInMinutes;
   uint8_t maxPrechillTimeForFreshFoodOnlyDefrostInMinutes;
   ValvePosition_t prechillRefrigerantValvePosition;
   uint8_t prechillCompressorSpeed;
   FanSpeed_t prechillFreezerFanSpeed;
   FanSpeed_t prechillFreshFoodFanSpeed;
   FanSpeed_t prechillIceBoxFanSpeed;
   FanSpeed_t prechillDeliFanSpeed;
   DamperPosition_t prechillConvertibleCompartmentDamperPosition;
   FanSpeed_t prechillConvertibleCompartmentFanSpeed;
   DamperPosition_t prechillFreshFoodDamperPosition;
   TemperatureDegFx100_t prechillFreezerMinTempInDegFx100;
   TemperatureDegFx100_t prechillFreshFoodMinTempInDegFx100;
   TemperatureDegFx100_t prechillFreshFoodMaxTempInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentAsFreezerMinTempInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100;
   TemperatureDegFx100_t prechillFreezerEvapExitTemperatureInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentEvapExitTemperatureInDegFx100;
   uint8_t defrostHeaterOnDelayAfterCompressorOffInSeconds;
   ValvePosition_t defrostHeaterOnRefrigerantValvePosition;
   uint8_t freezerDefrostHeaterMaxOnTimeInMinutes;
   uint8_t freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes;
   uint8_t freezerHeaterOnTimeToSetAbnormalDefrostInMinutes;
   TemperatureDegFx100_t freezerDefrostTerminationTemperatureInDegFx100;
   TemperatureDegFx100_t freshFoodDefrostTerminationTemperatureInDegFx100;
   TemperatureDegFx100_t convertibleCompartmentDefrostTerminationTemperatureInDegFx100;
   uint8_t freshFoodDefrostHeaterMaxOnTimeInMinutes;
   uint8_t freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes;
   uint8_t freshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes;
   uint8_t convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes;
   uint8_t convertibleCompartmentAsFreshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes;
   uint8_t convertibleCompartmentAsFreezerHeaterOnTimeToSetAbnormalDefrostInMinutes;
   uint8_t convertibleCompartmentAsFreshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes;
   uint8_t convertibleCompartmentAsFreezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes;
   uint8_t defrostDwellTimeInMinutes;
   ValvePosition_t dwellRefrigerantValvePosition;
   uint8_t postDwellExitTimeInMinutes;
   uint8_t freshFoodOnlyPostDwellExitTimeInMinutes;
   TemperatureDegFx100_t postDwellFreezerEvapExitTemperatureInDegFx100;
   TemperatureDegFx100_t freshFoodOnlyPostDwellFreezerEvapExitTemperatureInDegFx100;
   ValvePosition_t postDwellRefrigerantValvePosition;
   uint8_t postDwellCompressorSpeed;
   FanSpeed_t postDwellCondenserFanSpeed;
} DefrostData_t;

#endif
