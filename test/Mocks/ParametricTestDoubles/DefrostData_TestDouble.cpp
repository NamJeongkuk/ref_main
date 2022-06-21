/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostData_TestDouble.h"
#include "Constants_Time.h"
#include <string.h>

void DefrostData_TestDouble_SetPrechillFreezerSetpointInDegFx100(DefrostData_t *defrostData, TemperatureDegFx100_t prechillFreezerSetpointInDegFx100)
{
   defrostData->prechillFreezerSetpointInDegFx100 = prechillFreezerSetpointInDegFx100;
}

void DefrostData_TestDouble_SetPrechillConvertibleCompartmentSetpointInDegFx100(DefrostData_t *defrostData, TemperatureDegFx100_t prechillConvertibleCompartmentSetpointInDegFx100)
{
   defrostData->prechillConvertibleCompartmentSetpointInDegFx100 = prechillConvertibleCompartmentSetpointInDegFx100;
}

void DefrostData_TestDouble_SetDefrostMaxHoldoffTimeInMinutes(DefrostData_t *defrostData, uint16_t defrostMaxHoldoffTimeInMinutes)
{
   defrostData->defrostMaxHoldoffTimeInMinutes = defrostMaxHoldoffTimeInMinutes;
}

void DefrostData_TestDouble_SetMaxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds(DefrostData_t *defrostData, uint16_t maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds)
{
   defrostData->maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds;
}

void DefrostData_TestDouble_SetFreshFoodOnlyPostDwellExitTimeInMinutes(DefrostData_t *defrostData, uint8_t freshFoodOnlyPostDwellExitTimeInMinutes)
{
   defrostData->freshFoodOnlyPostDwellExitTimeInMinutes = freshFoodOnlyPostDwellExitTimeInMinutes;
}

void DefrostData_TestDouble_SetThreeWayValveTimePriorToPrechillCountsAsPrechillTime(DefrostData_t *defrostData, bool threeWayValveTimePriorToPrechillCountsAsPrechillTime)
{
   defrostData->threeWayValveTimePriorToPrechillCountsAsPrechillTime = threeWayValveTimePriorToPrechillCountsAsPrechillTime;
}

void DefrostData_TestDouble_Init(DefrostData_t *defrostData)
{
   memset(defrostData, 0, sizeof(DefrostData_t));

   defrostData->freezerDoorIncrementFactorInSecondsPerSecond = 348;
   defrostData->freshFoodDoorIncrementFactorInSecondsPerSecond = 87;
   defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR;
   defrostData->maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR;
   defrostData->prechillFreezerSetpointInDegFx100 = -600;
   defrostData->prechillFreshFoodSetpointInDegFx100 = 4600;
   defrostData->prechillConvertibleCompartmentSetpointInDegFx100 = -600;
   defrostData->prechillFreezerEvapExitTemperatureInDegFx100 = -3000;
   defrostData->prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000;
   defrostData->maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes = 10;
   defrostData->maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20;
   defrostData->defrostDoorHoldoffTimeForFreshFoodAndFreezerInMinutes = 60;
   defrostData->defrostDoorHoldoffTimeForFreshFoodOnlyInMinutes = 50;
   defrostData->defrostMaxHoldoffTimeInMinutes = 60;
   defrostData->maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = 0;
   defrostData->freshFoodFanDefrostFreshFoodEvapExitTemperatureInDegFx100 = 3600;
   defrostData->freshFoodFanDefrostFreshFoodFanMaxOnTimeInMinutes = 10;
   defrostData->convertibleCompartmentFanDefrostConvertibleCompartmentEvapExitTemperatureInDegFx100 = 3200;
   defrostData->convertibleCompartmentFanDefrostConvertibleCompartmentFanMaxOnTimeInMinutes = 10;
   defrostData->freezerDefrostHeaterMaxOnTimeInMinutes = 60;
   defrostData->freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 30;
   defrostData->freezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 32;
   defrostData->freezerDefrostTerminationTemperatureInDegFx100 = 5900;
   defrostData->freshFoodDefrostTerminationTemperatureInDegFx100 = 4460;
   defrostData->convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460;
   defrostData->freshFoodDefrostHeaterMaxOnTimeInMinutes = 60;
   defrostData->freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20;
   defrostData->freshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21;
   defrostData->convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60;
   defrostData->convertibleCompartmentAsFreshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21;
   defrostData->convertibleCompartmentAsFreezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 35;
   defrostData->defrostDwellTimeInMinutes = 7;
   defrostData->freshFoodAndFreezerPostDwellFreezerExitTemperatureInDegFx100 = -1000;
   defrostData->dwellThreeWayValvePosition = ValvePosition_A;
   defrostData->postDwellThreeWayValvePositionForFreshFoodAndFreezer = ValvePosition_A;
   defrostData->freshFoodPostDefrostPullDownExitTemperatureInDegFx100 = 4000;
   defrostData->freezerPostDefrostPullDownExitTemperatureInDegFx100 = 4000;
   defrostData->numberOfFreshFoodDefrostsBeforeFreezerDefrost = 2;
   defrostData->numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost = 1;
   defrostData->freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes = 10;
   defrostData->freshFoodOnlyPostDwellExitTimeInMinutes = 10;
   defrostData->defrostPeriodicTimeoutInSeconds = 1;
   defrostData->threeWayValvePositionToExitIdle = ValvePosition_B;
   defrostData->threeWayValvePositionForMaxPrechillHoldoff = ValvePosition_B;
   defrostData->threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost = ValvePosition_B;
   defrostData->threeWayValvePositionToCountAsPrechillTime = ValvePosition_B;
   defrostData->threeWayValveTimePriorToPrechillCountsAsPrechillTime = true;
}
