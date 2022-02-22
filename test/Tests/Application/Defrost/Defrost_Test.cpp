/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Defrost.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "ValvePosition.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

static const DefrostData_t defrostData = {
   .fzDoorIncrementCount = 348,
   .ffDoorIncrementCount = 87,
   .fzAbnormalRunTimeInMinutes = 6,
   .maxTimeBetweenDefrostsInMinutes = 32,
   .dmFzDefrostTemperatureInDegFx100 = 1500,
   .prechillFzSetpointInDegFx100 = 600,
   .prechillFfSetpointInDegFx100 = 4600,
   .prechillCcSetpointInDegFx100 = 600,
   .prechillFzEvapExitTemperatureInDegFx100 = 3000,
   .prechillCcEvapExitTemperatureInDegFx100 = 3000,
   .maxPrechillTimeInMinutes = 10,
   .defrostDoorHoldoffTimeForFfAndFzInMinutes = 60,
   .defrostMaxHoldoffTimeInMinutes = 60,
   .maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = 0,
   .ffFanDefrostFfEvapExitTemperatureInDegFx100 = 3600,
   .ffFanDefrostFfFanMaxOnTimeInMinutes = 10,
   .ccFanDefrostCcEvapExitTemperatureInDegFx100 = 3200,
   .ccFanDefrostCcFanMaxOnTimeInMinutes = 10,
   .fzDefrostHeaterMaxOnTimeInMinutes = 60,
   .fzAbnormalDefrostHeaterMaxOnTimeInMinutes = 32,
   .fzDefrostTerminationTemperatureInDegFx100 = 5900,
   .ffDefrostTerminationTemperatureInDegFx100 = 4460,
   .ccDefrostTerminationTemperatureInDegFx100 = 4460,
   .ffDefrostHeaterMaxOnTimeInMinutes = 60,
   .ffAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .ccDefrostHeaterMaxOnTimeInMinutes = 60,
   .ccAsFfAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .ccAsFzAbnormalDefrostHeaterMaxOnTimeInMinutes = 35,
   .defrostDwellTimeInMinutes = 7,
   .ffAndFzPostDwellFzExitTemperatureInDegFx100 = 1000,
   .ffAndFzPostDwellFzExitTimeInMinutes = 10,
   .postDwellValvePositionForFfAndFz = ValvePos_A,
   .ffPostDefrostShiftInDegFx100 = 0,
   .fzPostDefrostShiftInDegFx100 = 0,
   .minimumFfPostDefrostPullDownTimeInSeconds = 0,
   .maximumFfPostDefrostPullDownTimeInSeconds = 0,
   .ffPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .minimumFzPostDefrostPullDownTimeInSeconds = 0,
   .maximumFzPostDefrostPullDownTimeInSeconds = 0,
   .fzPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .numberOfFfDefrostsBeforeFzDefrost = 2,
   .numberOfFfDefrostsBeforeAbnormalFzDefrost = 1,
   .doorHoldoffTimeForFfAndFzInMinutes = 60,
   .maxFzPrechillTimeForFfOnlyDefrostInMinutes = 0,
   .ffOnlyPostDwellExitTimeInMinutes = 10,
   .dsmFzSetpointTemperatureInDegFx100 = 200,
   .defrostPeriodicTimeoutInSeconds = 60,
};

TEST_GROUP(Defrost)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   Defrost_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }
};

TEST(Defrost, ShouldInit)
{
   Defrost_Init(&instance, dataModel);
}
