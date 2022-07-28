/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostData_TestDouble.h"
#include "Constants_Time.h"
#include <string.h>

void DefrostData_TestDouble_SetFreshFoodOnlyPostDwellExitTimeInMinutes(DefrostData_t *defrostData, uint8_t freshFoodOnlyPostDwellExitTimeInMinutes)
{
   defrostData->freshFoodOnlyPostDwellExitTimeInMinutes = freshFoodOnlyPostDwellExitTimeInMinutes;
}

void DefrostData_TestDouble_Init(DefrostData_t *defrostData)
{
   memset(defrostData, 0, sizeof(DefrostData_t));

   defrostData->defrostPeriodicTimeoutInSeconds = 1;
   defrostData->freezerDoorIncrementFactorInSecondsPerSecond = 348;
   defrostData->freshFoodDoorIncrementFactorInSecondsPerSecond = 87;
   defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR;
   defrostData->maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR;
   defrostData->numberOfFreshFoodDefrostsBeforeFreezerDefrost = 2;
   defrostData->numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost = 1;
   defrostData->maxPrechillPrepTimeInMinutes = 5;
   defrostData->maxPrechillTimeInMinutes = 10;
   defrostData->maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20;
   defrostData->prechillRefrigerantValvePosition = ValvePosition_B;
   defrostData->prechillCompressorSpeed = 0;
   defrostData->prechillFreezerFanSpeed = FanSpeed_Low;
   defrostData->prechillFreshFoodFanSpeed = FanSpeed_Low;
   defrostData->prechillIceBoxFanSpeed = FanSpeed_High;
   defrostData->prechillDeliFanSpeed = FanSpeed_Medium;
   defrostData->prechillConvertibleCompartmentDamperPosition = DamperPosition_Open;
   defrostData->prechillConvertibleCompartmentFanSpeed = FanSpeed_Low;
   defrostData->prechillFreshFoodDamperPosition = DamperPosition_Open;
   defrostData->prechillFreezerMinTempInDegFx100 = -600;
   defrostData->prechillFreshFoodMinTempInDegFx100 = 3200;
   defrostData->prechillFreshFoodMaxTempInDegFx100 = 4600;
   defrostData->prechillConvertibleCompartmentAsFreezerMinTempInDegFx100 = -600;
   defrostData->prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 = 3200;
   defrostData->prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100 = 4600;
   defrostData->prechillFreezerEvapExitTemperatureInDegFx100 = -3000;
   defrostData->prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000;
   defrostData->defrostHeaterOnDelayAfterCompressorOffInSeconds = 2;
   defrostData->defrostHeaterOnRefrigerantValvePosition = ValvePosition_A;
   defrostData->freezerDefrostHeaterMaxOnTimeInMinutes = 60;
   defrostData->freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 30;
   defrostData->freezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 32;
   defrostData->freezerDefrostTerminationTemperatureInDegFx100 = 5900;
   defrostData->freshFoodDefrostTerminationTemperatureInDegFx100 = 4460;
   defrostData->convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460;
   defrostData->freshFoodDefrostHeaterMaxOnTimeInMinutes = 60;
   defrostData->freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20;
   defrostData->freshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21;
   defrostData->convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60;
   defrostData->convertibleCompartmentAsFreshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21;
   defrostData->convertibleCompartmentAsFreezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 35;
   defrostData->convertibleCompartmentAsFreshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 2;
   defrostData->convertibleCompartmentAsFreezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 25;
   defrostData->defrostDwellTimeInMinutes = 7;
   defrostData->dwellRefrigerantValvePosition = ValvePosition_B;
   defrostData->postDwellExitTimeInMinutes = 10;
   defrostData->freshFoodOnlyPostDwellExitTimeInMinutes = 10;
   defrostData->postDwellFreezerEvapExitTemperatureInDegFx100 = -1000;
   defrostData->freshFoodOnlyPostDwellFreezerEvapExitTemperatureInDegFx100 = -1200;
   defrostData->postDwellRefrigerantValvePosition = ValvePosition_B;
   defrostData->postDwellCompressorSpeed = 200;
   defrostData->postDwellCondenserFanSpeed = FanSpeed_Low;
}
