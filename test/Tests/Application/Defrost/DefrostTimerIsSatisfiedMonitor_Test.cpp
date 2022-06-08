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
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .prechillFreezerSetpointInDegFx100 = 600,
   .prechillFreshFoodSetpointInDegFx100 = 4600,
   .prechillConvertibleCompartmentSetpointInDegFx100 = 600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
   .defrostDoorHoldoffTimeForFreshFoodAndFreezerInMinutes = 60,
   .defrostDoorHoldoffTimeForFreshFoodOnlyInMinutes = 50,
   .defrostMaxHoldoffTimeInMinutes = 60,
   .maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = 0,
   .freshFoodFanDefrostFreshFoodEvapExitTemperatureInDegFx100 = 3600,
   .freshFoodFanDefrostFreshFoodFanMaxOnTimeInMinutes = 10,
   .convertibleCompartmentFanDefrostConvertibleCompartmentEvapExitTemperatureInDegFx100 = 3200,
   .convertibleCompartmentFanDefrostConvertibleCompartmentFanMaxOnTimeInMinutes = 10,
   .freezerDefrostHeaterMaxOnTimeInMinutes = 60,
   .freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 30,
   .freezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
   .freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20,
   .freshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60,
   .convertibleCompartmentAsFreshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .convertibleCompartmentAsFreezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 35,
   .defrostDwellTimeInMinutes = 7,
   .freshFoodAndFreezerPostDwellFreezerExitTemperatureInDegFx100 = -1000,
   .dwellThreeWayValvePosition = ValvePosition_A,
   .postDwellThreeWayValvePositionForFreshFoodAndFreezer = ValvePosition_A,
   .freshFoodPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .freezerPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 2,
   .numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost = 1,
   .freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes = 10,
   .freshFoodOnlyPostDwellExitTimeInMinutes = 10,
   .defrostPeriodicTimeoutInSeconds = 1,
   .threeWayValvePositionToExitIdle = ValvePosition_B,
   .threeWayValvePositionForMaxPrechillHoldoff = ValvePosition_B,
   .threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost = ValvePosition_B,
   .threeWayValvePositionToCountAsPrechillTime = ValvePosition_B,
   .threeWayValveTimePriorToPrechillCountsAsPrechillTime = true
};

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 33 * MINUTES_PER_HOUR
};

#define CountLessThanFreezerAbnormalDefrostRunTimeInSeconds (defrostData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1)
#define CountEqualToFreezerAbnormalDefrostRunTimeInSeconds (defrostData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
#define CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds (defrostData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1)
#define CountBetweenMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
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
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
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

   void LastFreezerDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void LastFreezerDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void LastFreshFoodDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void LastFreshFoodDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
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

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFreezerAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFreezerDefrostWasAbnormalAndSabbathIsDisabled)
{
   Given LastFreezerDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesGreaterThanFreezerAbnormalRunTimeButLessThanMaxTimeBetweenDefrostsAndDemandResponseLevelIsOffAndLastFreezerDefrostWasAbnormalAndSabbathIsDisabled)
{
   Given LastFreezerDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFreezerAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFreshFoodDefrostWasAbnormalAndSabbathIsDisabled)
{
   Given LastFreshFoodDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFreezerAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFreezerDefrostWasAbnormalAndSabbathIsEnabled)
{
   Given LastFreezerDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFreezerAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFreshFoodDefrostWasAbnormalAndSabbathIsEnabled)
{
   Given LastFreshFoodDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFreezerAbnormalRunTimeAndDemandResponseLevelIsNotOffAndLastFreezerDefrostWasAbnormalAndSabbathIsDisabled)
{
   Given LastFreezerDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Low);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedErdWhenDefrostTimerCountReachesFreezerAbnormalRunTimeAndDemandResponseLevelIsOffAndLastFreshFoodAndFreezerDefrostsWereNormalAndSabbathIsDisabled)
{
   Given LastFreezerDefrostWasNormal();
   And LastFreshFoodDefrostWasNormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
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
   And LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldNotSetDefrostTimerIsSatisfiedUntilDefrostTimerCountReachesSabbathMaxTimeBetweenDefrosts)
{
   Given DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(ENABLED);
   And LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   When DefrostTimerCountIs(CountLessThanSabbathMaxTimeBetweenDefrostsInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToSabbathMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldOnlySetDefrostTimerIsSatisfiedOnceWhenCountBecomesGreaterThanFreezerAbnormalRunTime)
{
   Given LastFreshFoodDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);

   DefrostTimerIsSatisfiedShouldNotBeWrittenAgain();
   When DefrostTimerCountIs(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);
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
   Given LastFreshFoodDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(CLEAR);
   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Disable, 1);
   And DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Enable, 2);

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerIsSatisfiedMonitor, ShouldBeAbleToSetDefrostTimerIsSatisfiedErdAfterBeingResetWhileEnabled)
{
   Given LastFreshFoodDefrostWasAbnormal();
   And DemandResponseLevelIs(EnergyDemandLevel_Off);
   And SabbathModeIs(DISABLED);
   And DefrostTimerIsSatisfiedMonitorIsEnabled();

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(CLEAR);
   When DefrostTimerIsSatisfiedMonitorRequestIs(DefrostTimerIsSatisfied_Reset, 1);

   When DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerIsSatisfiedErdShouldBe(CLEAR);

   DefrostTimerIsSatisfiedShouldBeWrittenWith(SET);
   When DefrostTimerCountIs(CountEqualToFreezerAbnormalDefrostRunTimeInSeconds);
}
