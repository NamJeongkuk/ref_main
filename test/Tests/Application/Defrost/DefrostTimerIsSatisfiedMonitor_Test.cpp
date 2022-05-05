/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostTimerIsSatisfiedMonitor.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

static const DefrostData_t defrostData = {
   .fzDoorIncrementFactorInSecondsPerSecond = 348,
   .ffDoorIncrementFactorInSecondsPerSecond = 87,
   .fzAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .dmFzDefrostTemperatureInDegFx100 = 1500,
   .prechillFzSetpointInDegFx100 = 600,
   .prechillFfSetpointInDegFx100 = 4600,
   .prechillCcSetpointInDegFx100 = 600,
   .prechillFzEvapExitTemperatureInDegFx100 = -3000,
   .prechillCcEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
   .defrostDoorHoldoffTimeForFfAndFzInMinutes = 60,
   .defrostDoorHoldoffTimeForFfOnlyInMinutes = 50,
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
   .ffAndFzPostDwellFzExitTemperatureInDegFx100 = -1000,
   .ffAndFzPostDwellFzExitTimeInMinutes = 10,
   .dwellThreeWayValvePosition = ValvePosition_A,
   .postDwellThreeWayValvePositionForFfAndFz = ValvePosition_A,
   .ffPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .fzPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .numberOfFfDefrostsBeforeFzDefrost = 2,
   .numberOfFfDefrostsBeforeAbnormalFzDefrost = 1,
   .doorHoldoffTimeForFfAndFzInMinutes = 60,
   .ffOnlyPostDwellExitTimeInMinutes = 10,
   .dsmFzSetpointTemperatureInDegFx100 = 200,
   .defrostPeriodicTimeoutInSeconds = 1,
   .threeWayValvePositionToExitIdle = ValvePosition_B,
   .threeWayValvePositionForMaxPrechillHoldoff = ValvePosition_B,
   .threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost = ValvePosition_B
};

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 33 * MINUTES_PER_HOUR
};

#define CountLessThanFzAbnormalDefrostRunTimeInSeconds (defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1)
#define CountEqualToFzAbnormalDefrostRunTimeInSeconds (defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
#define CountGreaterThanFzAbnormalDefrostRunTimeInSeconds (defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1)
#define CountBetweenMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
#define CountLessThanMaxTimeBetweenDefrostsInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - 1)
#define CountEqualToMaxTimeBetweenDefrostsInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE)
#define CountGreaterThanMaxTimeBetweenDefrostsInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE + 1)

#define CountLessThanSabbathMaxTimeBetweenDefrostsInSeconds (sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - 1)
#define CountEqualToSabbathMaxTimeBetweenDefrostsInSeconds (sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE)
#define CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds (sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE + 1)

static const DefrostTimerIsSatisfiedMonitorConfig_t config = {
   .defrostTimerIsSatisfiedMonitorFsmStateErd = Erd_DefrostTimerIsSatisfiedMonitorFsmState,
   .ramDefrostTimerCountInSecondsErd = Erd_DefrostTimerCountInSeconds,
   .defrostTimerIsSatisfiedMonitorRequestErd = Erd_DefrostTimerIsSatisfiedMonitorRequest,
   .defrostTimerIsSatisfiedErd = Erd_DefrostTimerIsSatisfied,
   .sabbathModeErd = Erd_SabbathMode,
   .fzDefrostWasAbnormalErd = Erd_FzDefrostWasAbnormal,
   .ffDefrostWasAbnormalErd = Erd_FfDefrostWasAbnormal,
   .demandResponseLevelErd = Erd_DemandResponseLevel,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes
};

TEST_GROUP(DefrostTimerIsSatisfiedMonitor)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DefrostTimerIsSatisfiedMonitor_t instance;
   EventSubscription_t defrostTimerIsSatisfiedSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetSabbath(&personalityParametricData, &sabbathData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void DefrostTimerIsSatisfiedMonitorIsInitialized()
   {
      DefrostTimerIsSatisfiedMonitor_Init(&instance, dataModel, &config);

      EventSubscription_Init(
         &defrostTimerIsSatisfiedSubscription,
         &instance,
         DefrostTimerIsSatisfiedChanged);

      DataModel_Subscribe(
         dataModel,
         Erd_DefrostTimerIsSatisfied,
         &defrostTimerIsSatisfiedSubscription);
   }

   void DefrostTimerIsSatisfiedFsmStateShouldBe(DefrostTimerIsSatisfiedMonitorFsmState_t expectedState)
   {
      DefrostTimerIsSatisfiedMonitorFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostTimerIsSatisfiedMonitorFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimer_t request, Signal_t id)
   {
      DefrostTimerIsSatisfiedMonitorRequest_t defrostTimerIsSatisfiedMonitorRequest;
      defrostTimerIsSatisfiedMonitorRequest.request = request;
      defrostTimerIsSatisfiedMonitorRequest.requestId = id;
      DataModel_Write(dataModel, Erd_DefrostTimerIsSatisfiedMonitorRequest, &defrostTimerIsSatisfiedMonitorRequest);
   }

   void DefrostTimerIsSatisfiedMonitorIsEnabled()
   {
      Given DefrostTimerIsSatisfiedMonitorIsInitialized();

      When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Enable, 1);
      DefrostTimerIsSatisfiedFsmStateShouldBe(DefrostTimerIsSatisfiedMonitorFsmState_Enabled);
   }

   void DefrostTimerIsSatisfiedErdIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostTimerIsSatisfied, &state);
   }

   void DefrostTimerIsSatisfiedErdShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_DefrostTimerIsSatisfied, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DefrostTimerCountIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostTimerCountInSeconds, &count);
   }

   void SabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathMode, &state);

      if(state)
      {
         DataModel_Write(dataModel, Erd_MaxTimeBetweenDefrostsInMinutes, &sabbathData.maxTimeBetweenDefrostsInMinutes);
      }
      else
      {
         DataModel_Write(dataModel, Erd_MaxTimeBetweenDefrostsInMinutes, &defrostData.maxTimeBetweenDefrostsInMinutes);
      }
   }

   void LastFzDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FzDefrostWasAbnormal, &state);
   }

   void LastFzDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FzDefrostWasAbnormal, &state);
   }

   void LastFfDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FfDefrostWasAbnormal, &state);
   }

   void LastFfDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FfDefrostWasAbnormal, &state);
   }

   void DemandResponseLevelIs(EnergyDemandLevel_t level)
   {
      DataModel_Write(dataModel, Erd_DemandResponseLevel, &level);
   }

   static void DefrostTimerIsSatisfiedChanged(void *context, const void *_args)
   {
      IGNORE(context);
      REINTERPRET(state, _args, const bool *);

      mock().actualCall("DefrostTimerIsSatisfiedChanged").withBoolParameter("state", *state);
   }

   void DefrostTimerIsSatisfiedShouldBeWrittenWith(bool state)
   {
      mock().expectOneCall("DefrostTimerIsSatisfiedChanged").withBoolParameter("state", state);
   };

   void DefrostTimerIsSatisfiedShouldNotBeWrittenAgain()
   {
      mock().expectNoCall("DefrostTimerIsSatisfiedChanged");
   };
};

TEST(DefrostTimerIsSatisfiedMonitor, ShouldInitializeAndBeDisabled)
{
   DefrostTimerIsSatisfiedMonitorIsInitialized();
   DefrostTimerIsSatisfiedFsmStateShouldBe(DefrostTimerIsSatisfiedMonitorFsmState_Disabled);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldEnableWhenEnableRequestReceivedWhileDisabled)
{
   Given DefrostTimerIsSatisfiedMonitorIsInitialized();

   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Enable, 1);
   DefrostTimerIsSatisfiedFsmStateShouldBe(DefrostTimerIsSatisfiedMonitorFsmState_Enabled);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotEnableWhenResetRequestReceivedWhileDisabled)
{
   Given DefrostTimerIsSatisfiedMonitorIsInitialized();

   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Reset, 1);
   DefrostTimerIsSatisfiedFsmStateShouldBe(DefrostTimerIsSatisfiedMonitorFsmState_Disabled);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldDisableWhenDisableRequestReceivedWhileEnabled)
{
   Given DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Disable, 1);
   DefrostTimerIsSatisfiedFsmStateShouldBe(DefrostTimerIsSatisfiedMonitorFsmState_Disabled);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldClearDefrostTimerIsSatisfiedErdWhenInitialized)
{
   Given DefrostTimerIsSatisfiedErdIs(SET);

   When DefrostTimerIsSatisfiedMonitorIsInitialized();
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldClearDefrostTimerIsSatisfiedErdWhenResetRequestReceivedWhileEnabled)
{
   Given DefrostTimerIsSatisfiedMonitorIsEnabled();
   And DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerIsSatisfiedErdIs(SET);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(CLEAR);
   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Reset, 1);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldClearDefrostTimerIsSatisfiedErdWhenDisableRequestReceivedWhileEnabled)
{
   Given DefrostTimerIsSatisfiedMonitorIsEnabled();
   And DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerIsSatisfiedErdIs(SET);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(CLEAR);
   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Disable, 1);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFzAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFzDefrostWasAbnormalAndSabbathIsDisabled)
{
   Given LastFzDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesGreaterThanFzAbnormalRunTimeButLessThanMaxTimeBetweenDefrostsAndDemandResponseLevelIsOffAndLastFzDefrostWasAbnormalAndSabbathIsDisabled)
{
   Given LastFzDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFzAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFfDefrostWasAbnormalAndSabbathIsDisabled)
{
   Given LastFfDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFzAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFzDefrostWasAbnormalAndSabbathIsEnabled)
{
   Given LastFzDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFzAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFfDefrostWasAbnormalAndSabbathIsEnabled)
{
   Given LastFfDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFzAbnormalRunTimeAndDemandResponseLevelIsNotOffAndLastFzDefrostWasAbnormalAndSabbathIsDisabled)
{
   Given LastFzDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Low);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFzAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFfAndFzDefrostsWereNormalAndSabbathIsDisabled)
{
   Given LastFzDefrostWasNormal();
   And LastFfDefrostWasNormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesMaxTimeBetweenDefrostsAndDemandResponseLevelIsOffAndSabbathModeIsDisabled)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesGreaterThanMaxTimeBetweenDefrostsAndDemandResponseLevelIsOffAndSabbathModeIsDisabled)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesSabbathMaxTimeBetweenDefrostsAndDemandResponseLevelIsOffAndSabbathModeIsEnabled)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanSabbathMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToSabbathMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesGreaterThanSabbathMaxTimeBetweenDefrostsAndDemandResponseLevelIsOffAndSabbathModeIsEnabled)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanSabbathMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesMaxTimeBetweenDefrostsAndDemandResponseLevelIsNotOffAndSabbathModeIsDisabled)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Low);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesSabbathMaxTimeBetweenDefrostsAndDemandResponseLevelIsNotOffAndSabbathModeIsEnabled)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Low);
   And SabbathModeIs(ENABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanSabbathMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToSabbathMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedUntilDefrostTimerCountReachesMaxTimeBetweenDefrosts)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And LastFfDefrostWasNormal();
   And LastFzDefrostWasNormal();
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedUntilDefrostTimerCountReachesSabbathMaxTimeBetweenDefrosts)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And LastFfDefrostWasNormal();
   And LastFzDefrostWasNormal();
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountLessThanSabbathMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToSabbathMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldOnlySetDefrostTimerIsSatisfiedOnceWhenCountBecomesGreaterThanFzAbnormalRunTime)
{
   Given LastFfDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);

   DefrostTimerIsSatisfiedShouldNotBeWrittenAgain();
   When DefrostTimerCountIs(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldOnlySetDefrostTimerIsSatisfiedOnceWhenCountBecomesGreaterThanMaxTimeBetweenDefrosts)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToMaxTimeBetweenDefrostsInSeconds);

   DefrostTimerIsSatisfiedShouldNotBeWrittenAgain();
   When DefrostTimerCountIs(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldOnlySetDefrostTimerIsSatisfiedOnceWhenCountBecomesGreaterThanSabbathMaxTimeBetweenDefrosts)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanSabbathMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToSabbathMaxTimeBetweenDefrostsInSeconds);

   DefrostTimerIsSatisfiedShouldNotBeWrittenAgain();
   When DefrostTimerCountIs(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldBeAbleToSetDefrostTimerIsSatisfiedErdAfterBeingDisabledThenReenabled)
{
   Given LastFfDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(CLEAR);
   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Disable, 1);
   And DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Enable, 2);

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldBeAbleToSetDefrostTimerIsSatisfiedErdAfterBeingResetWhileEnabled)
{
   Given LastFfDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(CLEAR);
   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Reset, 1);

   When DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFzAbnormalDefrostRunTimeInSeconds);
}
