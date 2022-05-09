/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostTimerCounter.h"
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

enum
{
   SomeDoorAccelerationCounts = 50
};

enum
{
   SomeDefrostCounts = 999
};

static const DefrostData_t defrostData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .freezerAbnormalRunTimeInMinutes = 6 * 60,
   .maxTimeBetweenDefrostsInMinutes = 32 * 60,
   .dmFreezerDefrostTemperatureInDegFx100 = 1500,
   .prechillFreezerSetpointInDegFx100 = 600,
   .prechillFreshFoodSetpointInDegFx100 = 4600,
   .prechillConvertibleCompartmentSetpointInDegFx100 = 600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeInMinutes = 10,
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
   .freezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
   .freshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60,
   .convertibleCompartmentAsFreshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .convertibleCompartmentAsFreezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 35,
   .defrostDwellTimeInMinutes = 7,
   .freshFoodAndFreezerPostDwellFreezerExitTemperatureInDegFx100 = -1000,
   .freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes = 10,
   .dwellThreeWayValvePosition = ValvePosition_A,
   .postDwellThreeWayValvePositionForFreshFoodAndFreezer = ValvePosition_A,
   .freshFoodPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .freezerPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 2,
   .numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost = 1,
   .doorHoldoffTimeForFreshFoodAndFreezerInMinutes = 60,
   .freshFoodOnlyPostDwellExitTimeInMinutes = 10,
   .dsmFreezerSetpointTemperatureInDegFx100 = 200,
   .defrostPeriodicTimeoutInSeconds = 1,
   .threeWayValvePositionToExitIdle = ValvePosition_B,
   .threeWayValvePositionForMaxPrechillHoldoff = ValvePosition_B,
   .threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost = ValvePosition_B
};

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 33 * MINUTES_PER_HOUR
};

#define CountLessThanFreezerAbnormalDefrostRunTimeInSeconds (defrostData.freezerAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 2)
#define CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds (defrostData.freezerAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1)
#define CountBetweenMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.freezerAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
#define CountGreaterThanMaxTimeBetweenDefrostsInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE + 1)

#define CountBetweenSabbathMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds (sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.freezerAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
#define CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds (sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE + 1)

static const DefrostTimerCounterConfig_t config = {
   .defrostTimerCounterFsmStateErd = Erd_DefrostTimerCounterFsmState,
   .defrostTimerCounterRequestErd = Erd_DefrostTimerCounterRequest,
   .ramDefrostTimerCountInSecondsErd = Erd_DefrostTimerCountInSeconds,
   .doorAccelerationRequestErd = Erd_DoorAccelerationRequest,
   .freshFoodDoorAccelerationCountsErd = Erd_DefrostFreshFoodDoorAccelerationCount,
   .freezerDoorAccelerationCountsErd = Erd_DefrostFreezerDoorAccelerationCount,
   .defrostTimerIsSatisfiedMonitorRequestErd = Erd_DefrostTimerIsSatisfiedMonitorRequest,
   .compressorResolvedSpeedErd = Erd_CompressorSpeed_ResolvedVote,
   .compressorSpeedConfigErd = Erd_CompressorSpeedConfig,
   .sabbathModeErd = Erd_SabbathMode,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes,
   .eepromDefrostTimerCountInSecondsErd = Erd_Eeprom_DefrostTimerCountInSeconds,
   .timerModuleErd = Erd_TimerModule,
};

TEST_GROUP(DefrostTimerCounter)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DefrostTimerCounter_t instance;

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

   void DefrostTimerCounterIsInitialized()
   {
      DefrostTimerCounter_Init(&instance, dataModel, &config);
   }

   void DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_t expectedState)
   {
      DefrostTimerCounterFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostTimerCounterFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DefrostTimerCounterRequestIs(DefrostTimer_t request, Signal_t id)
   {
      DefrostTimerCounterRequest_t defrostTimerCounterRequest;
      defrostTimerCounterRequest.request = request;
      defrostTimerCounterRequest.requestId = id;
      DataModel_Write(dataModel, Erd_DefrostTimerCounterRequest, &defrostTimerCounterRequest);
   }

   void DefrostTimerCountIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostTimerCountInSeconds, &count);
   }

   void EepromDefrostTimerCountIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_Eeprom_DefrostTimerCountInSeconds, &count);
   }

   void DefrostTimerCountShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostTimerCountInSeconds, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void EepromDefrostTimerCountShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_Eeprom_DefrostTimerCountInSeconds, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void DoorAccelerationRequestShouldBe(DoorAcceleration_t request, Signal_t id)
   {
      DoorAccelerationRequest_t doorAccelerationRequest;
      DataModel_Read(dataModel, Erd_DoorAccelerationRequest, &doorAccelerationRequest);

      CHECK_EQUAL(request, doorAccelerationRequest.request);
      CHECK_EQUAL(id, doorAccelerationRequest.requestId);
   }

   void DefrostTimerIsSatisfiedMonitorRequestShouldBe(DefrostTimerIsSatisfied_t request, Signal_t id)
   {
      DefrostTimerIsSatisfiedMonitorRequest_t defrostTimerIsSatisfiedMonitorRequest;
      DataModel_Read(dataModel, Erd_DefrostTimerIsSatisfiedMonitorRequest, &defrostTimerIsSatisfiedMonitorRequest);

      CHECK_EQUAL(request, defrostTimerIsSatisfiedMonitorRequest.request);
      CHECK_EQUAL(id, defrostTimerIsSatisfiedMonitorRequest.requestId);
   }

   void DefrostTimerIsEnabled()
   {
      Given DefrostTimerCounterIsInitialized();

      When DefrostTimerCounterRequestIs(DefrostTimer_Enable, 1);
      DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Enabled);
   }

   void SingleSpeedCompressorIsOn()
   {
      CompressorIsSingleSpeed();

      CompressorVotedSpeed_t compressorVotedSpeed;
      compressorVotedSpeed.care = true;
      compressorVotedSpeed.speed = CompressorSpeed_On;

      DataModel_Write(dataModel, Erd_CompressorSpeed_ResolvedVote, &compressorVotedSpeed);
   }

   void SingleSpeedCompressorIsOff()
   {
      CompressorIsSingleSpeed();

      CompressorVotedSpeed_t compressorVotedSpeed;
      compressorVotedSpeed.care = true;
      compressorVotedSpeed.speed = CompressorSpeed_Off;

      DataModel_Write(dataModel, Erd_CompressorSpeed_ResolvedVote, &compressorVotedSpeed);
   }

   void VariableSpeedCompressorIsOn()
   {
      CompressorIsVariableSpeed();

      CompressorVotedSpeed_t compressorVotedSpeed;
      compressorVotedSpeed.care = true;
      compressorVotedSpeed.speed = CompressorSpeed_Mid;

      DataModel_Write(dataModel, Erd_CompressorSpeed_ResolvedVote, &compressorVotedSpeed);
   };

   void VariableSpeedCompressorIsOff()
   {
      CompressorIsVariableSpeed();

      CompressorVotedSpeed_t compressorVotedSpeed;
      compressorVotedSpeed.care = true;
      compressorVotedSpeed.speed = CompressorSpeed_Off;

      DataModel_Write(dataModel, Erd_CompressorSpeed_ResolvedVote, &compressorVotedSpeed);
   };

   void CompressorIsSingleSpeed()
   {
      CompressorSpeedConfig_t config = CompressorSpeedConfig_SingleSpeed;
      DataModel_Write(dataModel, Erd_CompressorSpeedConfig, &config);
   }

   void CompressorIsVariableSpeed()
   {
      CompressorSpeedConfig_t config = CompressorSpeedConfig_VariableSpeed;
      DataModel_Write(dataModel, Erd_CompressorSpeedConfig, &config);
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

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void FreezerDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreezerDoorAccelerationCount, &count);
   }

   void FreshFoodDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFreshFoodDoorAccelerationCount, &count);
   }
};

TEST(DefrostTimerCounter, ShouldInitializeAndBeDisabled)
{
   Given DefrostTimerCounterIsInitialized();
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);
}

TEST(DefrostTimerCounter, ShouldEnableWhenEnableRequestReceivedWhileDisabled)
{
   Given DefrostTimerCounterIsInitialized();

   When DefrostTimerCounterRequestIs(DefrostTimer_Enable, 1);
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Enabled);
}

TEST(DefrostTimerCounter, ShouldNotEnableWhenResetRequestReceivedWhileDisabled)
{
   Given DefrostTimerCounterIsInitialized();

   When DefrostTimerCounterRequestIs(DefrostTimer_Reset, 1);
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);
}

TEST(DefrostTimerCounter, ShouldDisableWhenDisableRequestReceivedWhileEnabled)
{
   Given DefrostTimerIsEnabled();

   When DefrostTimerCounterRequestIs(DefrostTimer_Disable, 2);
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);
}

TEST(DefrostTimerCounter, ShouldSendDisableDoorAccelerationRequestOnInit)
{
   Given DefrostTimerCounterIsInitialized();
   DoorAccelerationRequestShouldBe(DoorAcceleration_Disable, 1);
}

TEST(DefrostTimerCounter, ShouldSendDisableDefrostTimerIsSatisfiedMonitorRequestOnInit)
{
   Given DefrostTimerCounterIsInitialized();
   DefrostTimerIsSatisfiedMonitorRequestShouldBe(DefrostTimerIsSatisfied_Disable, 1);
}

TEST(DefrostTimerCounter, ShouldSendDefrostTimerIsSatisfiedRequestWhenEnabled)
{
   Given DefrostTimerIsEnabled();
   DefrostTimerIsSatisfiedMonitorRequestShouldBe(DefrostTimerIsSatisfied_Enable, 2);
}

TEST(DefrostTimerCounter, ShouldSendEnableDoorAccelerationRequestWhenEnabled)
{
   Given DefrostTimerIsEnabled();
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}

TEST(DefrostTimerCounter, ShouldNotResetDefrostTimerCountToZeroWhenDisabled)
{
   Given DefrostTimerIsEnabled();

   When DefrostTimerCountIs(SomeDefrostCounts);
   And DefrostTimerCounterRequestIs(DefrostTimer_Disable, 2);

   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);
   DefrostTimerCountShouldBe(SomeDefrostCounts);
}

TEST(DefrostTimerCounter, ShouldSendDisableDoorAccelerationRequestWhenDisabled)
{
   Given DefrostTimerIsEnabled();

   When DefrostTimerCounterRequestIs(DefrostTimer_Disable, 2);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Disable, 3);
}

TEST(DefrostTimerCounter, ShouldSendDisableDefrostTimerIsSatisfiedMonitorRequestWhenDisabled)
{
   Given DefrostTimerIsEnabled();

   When DefrostTimerCounterRequestIs(DefrostTimer_Disable, 2);
   DefrostTimerIsSatisfiedMonitorRequestShouldBe(DefrostTimerIsSatisfied_Disable, 3);
}

TEST(DefrostTimerCounter, ShouldResetDefrostTimerCountToZeroWhenRequestedToResetWhileEnabled)
{
   Given DefrostTimerIsEnabled();

   When DefrostTimerCountIs(SomeDefrostCounts);
   And DefrostTimerCounterRequestIs(DefrostTimer_Reset, 2);

   DefrostTimerCountShouldBe(0);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenRequestedToResetWhileEnabled)
{
   Given DefrostTimerIsEnabled();

   And DefrostTimerCounterRequestIs(DefrostTimer_Reset, 2);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDefrostTimerIsSatisfiedMonitorRequestWhenRequestedToResetWhileEnabled)
{
   Given DefrostTimerIsEnabled();

   And DefrostTimerCounterRequestIs(DefrostTimer_Reset, 2);
   DefrostTimerIsSatisfiedMonitorRequestShouldBe(DefrostTimerIsSatisfied_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldIncrementDefrostTimerCountWhenEnabledAndInSabbathModeAfterPeriodicTimeout)
{
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(0);
   Given SabbathModeIs(ENABLED);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(0);

   After(1);
   DefrostTimerCountShouldBe(1);
}

TEST(DefrostTimerCounter, ShouldIncrementDefrostTimerCountMultipleTimesWhenEnabledAndInSabbathModeAfterPeriodicTimeouts)
{
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(0);
   Given SabbathModeIs(ENABLED);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
      DefrostTimerCountShouldBe(i);

      After(1);
      DefrostTimerCountShouldBe(i + 1);
   }
}

TEST(DefrostTimerCounter, ShouldIncrementDefrostTimerCountWhenEnabledAndNotInSabbathModeButSingleSpeedCompressorIsOnAfterPeriodicTimeout)
{
   Given SingleSpeedCompressorIsOn();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(0);
   Given SabbathModeIs(DISABLED);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(0);

   After(1);
   DefrostTimerCountShouldBe(1);
}

TEST(DefrostTimerCounter, ShouldIncrementDefrostTimerCountWhenEnabledAndNotInSabbathModeButVariableSpeedCompressorIsOnAfterPeriodicTimeout)
{
   Given VariableSpeedCompressorIsOn();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(0);
   Given SabbathModeIs(DISABLED);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(0);

   After(1);
   DefrostTimerCountShouldBe(1);
}

TEST(DefrostTimerCounter, ShouldIncrementDefrostTimerCountWhenEnabledAndInSabbathModeAndSingleSpeedCompressorIsOnAfterPeriodicTimeout)
{
   Given SingleSpeedCompressorIsOn();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(0);
   Given SabbathModeIs(ENABLED);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(0);

   After(1);
   DefrostTimerCountShouldBe(1);
}

TEST(DefrostTimerCounter, ShouldIncrementDefrostTimerCountWhenEnabledAndInSabbathModeAndVariableSpeedCompressorIsOnAfterPeriodicTimeout)
{
   Given VariableSpeedCompressorIsOn();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(0);
   Given SabbathModeIs(ENABLED);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(0);

   After(1);
   DefrostTimerCountShouldBe(1);
}

TEST(DefrostTimerCounter, ShouldNotIncrementDefrostTimerCountWhenEnabledAndNotInSabbathModeAndSingleSpeedCompressorIsOffAfterPeriodicTimeout)
{
   Given SingleSpeedCompressorIsOff();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(0);
   Given SabbathModeIs(DISABLED);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(0);

   After(1);
   DefrostTimerCountShouldBe(0);
}

TEST(DefrostTimerCounter, ShouldNotIncrementDefrostTimerCountWhenEnabledAndNotInSabbathModeAndVariableSpeedCompressorIsOffAfterPeriodicTimeout)
{
   Given VariableSpeedCompressorIsOff();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(0);
   Given SabbathModeIs(DISABLED);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(0);

   After(1);
   DefrostTimerCountShouldBe(0);
}

// Sabbath mode disabled
TEST(DefrostTimerCounter, ShouldAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsBetweenFreezerAbnormalRunTimeAndMaxTimeBetweenDefrosts)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountBetweenMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountBetweenMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountBetweenMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds + 2 * SomeDoorAccelerationCounts);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsEqualToMaxTimeBetweenDefrosts)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(defrostData.maxTimeBetweenDefrostsInMinutes);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(defrostData.maxTimeBetweenDefrostsInMinutes);

   After(1);
   DefrostTimerCountShouldBe(defrostData.maxTimeBetweenDefrostsInMinutes);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsGreaterThanMaxTimeBetweenDefrosts)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasAbnormalAndDefrostTimerCountIsGreaterThanFreezerAbnormalDefrostRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsLessThanFreezerAbnormalDefrostRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsEqualToFreezerAbnormalRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(defrostData.freezerAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsEqualToMaxTimeBetweenDefrost)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsGreaterThanMaxTimeBetweenDefrost)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldNotSendResetDoorAccelerationRequestWhenLastFreezerDefrostWasAbnormalAndDefrostTimerCountIsGreaterThanFreezerAbnormalRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}

TEST(DefrostTimerCounter, ShouldNotSendResetDoorAccelerationRequestWhenDefrostTimerCounterIsLessThanFreezerAbnormalRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}

// Sabbath mode enabled
TEST(DefrostTimerCounter, ShouldAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsBetweenFreezerAbnormalRunTimeAndSabbathMaxTimeBetweenDefrostsWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountBetweenSabbathMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountBetweenSabbathMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountBetweenSabbathMaxTimeBetweenDefrostsAndFreezerAbnormalDefrostRunTimeInSeconds + 2 * SomeDoorAccelerationCounts + 1);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsEqualToSabbathMaxTimeBetweenDefrostsWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(sabbathData.maxTimeBetweenDefrostsInMinutes);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(sabbathData.maxTimeBetweenDefrostsInMinutes);

   After(1);
   DefrostTimerCountShouldBe(sabbathData.maxTimeBetweenDefrostsInMinutes + 1);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsGreaterThanSabbathMaxTimeBetweenDefrostsWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds + 1);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasAbnormalAndDefrostTimerCountIsGreaterThanFreezerAbnormalDefrostRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds + 1);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsLessThanFreezerAbnormalDefrostRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FreshFoodDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FreezerDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds + 1);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsEqualToFreezerAbnormalRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(defrostData.freezerAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsEqualToSabbathMaxTimeBetweenDefrostWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFreezerDefrostWasNormalAndDefrostTimerCountIsGreaterThanSabbathMaxTimeBetweenDefrostWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldNotSendResetDoorAccelerationRequestWhenLastFreezerDefrostWasAbnormalAndDefrostTimerCountIsGreaterThanFreezerAbnormalRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}

TEST(DefrostTimerCounter, ShouldNotSendResetDoorAccelerationRequestWhenDefrostTimerCounterIsLessthanFreezerAbnormalRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFreezerDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}

TEST(DefrostTimerCounter, ShouldResetEepromDefrostCountAndRamDefrostCountWhenDefrostTimerCounterIsResetWhileEnabled)
{
   Given DefrostTimerIsEnabled();
   And EepromDefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   And DefrostTimerCountIs(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   When DefrostTimerCounterRequestIs(DefrostTimer_Reset, 1);
   DefrostTimerCountShouldBe(0);
   EepromDefrostTimerCountShouldBe(0);
}

TEST(DefrostTimerCounter, ShouldNotResetDefrostCountsToZeroWhenDisabledAndTheyShouldBeTheSameWhenEnabledAgain)
{
   Given DefrostTimerIsEnabled();
   And EepromDefrostTimerCountIs(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   And DefrostTimerCountIs(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   When DefrostTimerCounterRequestIs(DefrostTimer_Disable, 2);
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);
   EepromDefrostTimerCountShouldBe(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerCountShouldBe(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);

   When DefrostTimerCounterRequestIs(DefrostTimer_Enable, 3);
   EepromDefrostTimerCountShouldBe(CountLessThanFreezerAbnormalDefrostRunTimeInSeconds);
   DefrostTimerCountShouldBe(CountGreaterThanFreezerAbnormalDefrostRunTimeInSeconds);
}
