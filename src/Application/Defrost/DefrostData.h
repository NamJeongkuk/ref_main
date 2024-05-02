/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTDATA_H
#define DEFROSTDATA_H

#include "TemperatureDegFx100.h"
#include "SealedSystemValvePosition.h"
#include "DamperVotedPosition.h"
#include "FanSpeed.h"
#include "CompressorVotedSpeed.h"

typedef struct
{
   uint16_t freezerDoorIncrementFactorInSecondsPerSecond;
   uint16_t freshFoodDoorIncrementFactorInSecondsPerSecond;
   uint16_t minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes;
   uint16_t maxTimeBetweenDefrostsInMinutes;
   uint16_t ahamPrechillTimeBetweenDefrostsInMinutes;
   uint8_t numberOfSecondaryOnlyDefrostsBeforeFullDefrost;
   uint8_t numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet;
} DefrostIdleData_t;

typedef struct
{
   uint8_t maxPrechillPrepTimeInMinutes;
} DefrostPrechillPrepData_t;

typedef struct
{
   uint8_t maxPrechillTimeInMinutes;
   uint8_t maxPrechillTimeForSecondaryOnlyDefrostInMinutes;
   SealedSystemValvePosition_t prechillSealedSystemValvePosition;
   CompressorSpeed_t prechillCompressorSpeed;
   FanSpeed_t prechillFreezerEvaporatorFanSpeed;
   FanSpeed_t prechillFreshFoodEvaporatorFanSpeed;
   FanSpeed_t prechillIceCabinetFanSpeed;
   FanSpeed_t prechillDeliFanSpeed;
   DamperPosition_t prechillConvertibleCompartmentDamperPosition;
   DamperPosition_t prechillDeliDamperPosition;
   FanSpeed_t prechillConvertibleCompartmentEvapFanSpeed;
   DamperPosition_t prechillFreshFoodDamperPosition;
   FanSpeed_t prechillCondenserFanSpeed;
   TemperatureDegFx100_t prechillFreezerMinTempInDegFx100;
   TemperatureDegFx100_t prechillFreshFoodMinTempInDegFx100;
   TemperatureDegFx100_t prechillFreshFoodMaxTempInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentAsFreezerMinTempInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100;
   TemperatureDegFx100_t prechillPrimaryEvaporatorExitTemperatureInDegFx100;
} DefrostPrechillData_t;

typedef struct
{
   uint8_t defrostHeaterOnDelayAfterCompressorOffInSeconds;
   DamperPosition_t heaterOnEntryFreshFoodDamperPosition;
   DamperPosition_t heaterOnEntryConvertibleCompartmentDamperPosition;
   SealedSystemValvePosition_t heaterOnEntrySealedSystemValvePosition;
} DefrostHeaterOnEntryData_t;

typedef struct
{
   uint8_t defrostHeaterMaxOnTimeInMinutes;
   uint8_t invalidThermistorDefrostHeaterMaxOnTimeInMinutes;
   uint8_t heaterOnTimeToSetAbnormalDefrostInMinutes;
   TemperatureDegFx100_t defrostTerminationTemperatureInDegFx100;
} DefrostHeaterData_t;

typedef struct
{
   uint8_t defrostHeaterMaxOnTimeInMinutes;
   TemperatureDegFx100_t defrostTerminationTemperatureInDegFx100;
} DefrostConvertibleCompartmentCommonHeaterData_t;

typedef struct
{
   uint8_t invalidThermistorDefrostHeaterMaxOnTimeInMinutes;
   uint8_t heaterOnTimeToSetAbnormalDefrostInMinutes;
} DefrostConvertibleCompartmentStateDependentHeaterData_t;

typedef struct
{
   DefrostHeaterData_t freshFoodHeater;
   DefrostHeaterData_t freezerHeater;
   DefrostConvertibleCompartmentCommonHeaterData_t convertibleCompartmentHeater;
   DefrostConvertibleCompartmentStateDependentHeaterData_t convertibleCompartmentHeaterAsFreshFood;
   DefrostConvertibleCompartmentStateDependentHeaterData_t convertibleCompartmentHeaterAsFreezer;
} DefrostHeaterOnData_t;

typedef struct
{
   uint8_t dwellTimeInMinutes;
   DamperPosition_t dwellFreshFoodDamperPosition;
   SealedSystemValvePosition_t dwellSealedSystemValvePosition;
} DefrostDwellData_t;

typedef struct
{
   uint8_t postDwellExitTimeInMinutes;
   uint8_t secondaryOnlyPostDwellExitTimeInMinutes;
   TemperatureDegFx100_t postDwellPrimaryEvaporatorExitTemperatureInDegFx100;
   TemperatureDegFx100_t secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100;
   SealedSystemValvePosition_t postDwellSealedSystemValvePosition;
   CompressorSpeed_t postDwellCompressorSpeed;
   FanSpeed_t postDwellCondenserFanSpeed;
   DamperPosition_t postDwellFreshFoodDamperPosition;
   DamperPosition_t postDwellConvertibleCompartmentDamperPosition;
   DamperPosition_t postDwellDeliDamperPosition;
} DefrostPostDwellData_t;

typedef struct
{
   DefrostIdleData_t idleData;
   DefrostPrechillPrepData_t prechillPrepData;
   DefrostPrechillData_t prechillData;
   DefrostHeaterOnEntryData_t heaterOnEntryData;
   DefrostHeaterOnData_t heaterOnData;
   DefrostDwellData_t dwellData;
   DefrostPostDwellData_t postDwellData;
} DefrostData_t;

#endif
