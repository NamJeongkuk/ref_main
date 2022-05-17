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
   uint16_t freezerDoorIncrementFactorInSecondsPerSecond;
   uint16_t freshFoodDoorIncrementFactorInSecondsPerSecond;
   uint16_t freezerAbnormalRunTimeInMinutes;
   uint16_t maxTimeBetweenDefrostsInMinutes;
   TemperatureDegFx100_t dmFreezerDefrostTemperatureInDegFx100;
   TemperatureDegFx100_t prechillFreezerSetpointInDegFx100;
   TemperatureDegFx100_t prechillFreshFoodSetpointInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentSetpointInDegFx100;
   TemperatureDegFx100_t prechillFreezerEvapExitTemperatureInDegFx100;
   TemperatureDegFx100_t prechillConvertibleCompartmentEvapExitTemperatureInDegFx100;
   uint8_t maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes;
   uint8_t maxPrechillTimeForFreshFoodOnlyDefrostInMinutes;
   uint8_t defrostDoorHoldoffTimeForFreshFoodAndFreezerInMinutes;
   uint8_t defrostDoorHoldoffTimeForFreshFoodOnlyInMinutes;
   uint16_t defrostMaxHoldoffTimeInMinutes;
   uint16_t maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds;
   TemperatureDegFx100_t freshFoodFanDefrostFreshFoodEvapExitTemperatureInDegFx100;
   uint16_t freshFoodFanDefrostFreshFoodFanMaxOnTimeInMinutes;
   TemperatureDegFx100_t convertibleCompartmentFanDefrostConvertibleCompartmentEvapExitTemperatureInDegFx100;
   uint16_t convertibleCompartmentFanDefrostConvertibleCompartmentFanMaxOnTimeInMinutes;
   uint8_t freezerDefrostHeaterMaxOnTimeInMinutes;
   uint8_t freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes;
   uint8_t freezerAbnormalDefrostHeaterMaxOnTimeInMinutes;
   TemperatureDegFx100_t freezerDefrostTerminationTemperatureInDegFx100;
   TemperatureDegFx100_t freshFoodDefrostTerminationTemperatureInDegFx100;
   TemperatureDegFx100_t convertibleCompartmentDefrostTerminationTemperatureInDegFx100;
   uint8_t freshFoodDefrostHeaterMaxOnTimeInMinutes;
   uint8_t freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes;
   uint8_t freshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes;
   uint8_t convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes;
   uint8_t convertibleCompartmentAsFreshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes;
   uint8_t convertibleCompartmentAsFreezerAbnormalDefrostHeaterMaxOnTimeInMinutes;
   uint8_t defrostDwellTimeInMinutes;
   TemperatureDegFx100_t freshFoodAndFreezerPostDwellFreezerExitTemperatureInDegFx100;
   ValvePosition_t dwellThreeWayValvePosition;
   ValvePosition_t postDwellThreeWayValvePositionForFreshFoodAndFreezer;
   TemperatureDegFx100_t freshFoodPostDefrostPullDownExitTemperatureInDegFx100;
   TemperatureDegFx100_t freezerPostDefrostPullDownExitTemperatureInDegFx100;
   uint8_t numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   uint8_t numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost;
   uint8_t freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes;
   uint8_t freshFoodOnlyPostDwellExitTimeInMinutes;
   TemperatureDegFx100_t dsmFreezerSetpointTemperatureInDegFx100;
   uint8_t defrostPeriodicTimeoutInSeconds;
   ValvePosition_t threeWayValvePositionToExitIdle;
   ValvePosition_t threeWayValvePositionForMaxPrechillHoldoff;
   ValvePosition_t threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost;
   ValvePosition_t threeWayValvePositionToCountAsPrechillTime;
   bool threeWayValveTimePriorToPrechillCountsAsPrechillTime;
} DefrostData_t;

#endif
