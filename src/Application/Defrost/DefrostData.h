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
#include "DamperVotedPosition.h"
#include "FanSpeed.h"
#include "CompressorVotedSpeed.h"

typedef struct
{
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
   CompressorSpeed_t prechillCompressorSpeed;
   FanSpeed_t prechillFreezerEvapFanSpeed;
   FanSpeed_t prechillFreshFoodEvapFanSpeed;
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
   uint8_t dwellTimeInMinutes;
   DamperPosition_t dwellFreshFoodDamperPosition;
   ValvePosition_t dwellRefrigerantValvePosition;
   uint8_t postDwellExitTimeInMinutes;
   uint8_t freshFoodOnlyPostDwellExitTimeInMinutes;
   TemperatureDegFx100_t postDwellFreezerEvapExitTemperatureInDegFx100;
   TemperatureDegFx100_t freshFoodOnlyPostDwellFreezerEvapExitTemperatureInDegFx100;
   ValvePosition_t postDwellRefrigerantValvePosition;
   CompressorSpeed_t postDwellCompressorSpeed;
   FanSpeed_t postDwellCondenserFanSpeed;
   DamperPosition_t postDwellFreshFoodDamperPosition;
} DefrostData_t;

#endif
