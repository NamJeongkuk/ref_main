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

static const DefrostData_t defrostData = {
   .fzDoorIncrementFactorInSecondsPerSecond = 348,
   .ffDoorIncrementFactorInSecondsPerSecond = 87,
   .fzAbnormalRunTimeInMinutes = 6 * 60,
   .maxTimeBetweenDefrostsInMinutes = 32 * 60,
   .dmFzDefrostTemperatureInDegFx100 = 1500,
   .prechillFzSetpointInDegFx100 = 600,
   .prechillFfSetpointInDegFx100 = 4600,
   .prechillCcSetpointInDegFx100 = 600,
   .prechillFzEvapExitTemperatureInDegFx100 = -3000,
   .prechillCcEvapExitTemperatureInDegFx100 = -3000,
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
   .ffAndFzPostDwellFzExitTemperatureInDegFx100 = -1000,
   .ffAndFzPostDwellFzExitTimeInMinutes = 10,
   .dwellThreeWayValvePosition = ValvePos_A,
   .postDwellThreeWayValvePositionForFfAndFz = ValvePos_A,
   .ffPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .fzPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .numberOfFfDefrostsBeforeFzDefrost = 2,
   .numberOfFfDefrostsBeforeAbnormalFzDefrost = 1,
   .doorHoldoffTimeForFfAndFzInMinutes = 60,
   .ffOnlyPostDwellExitTimeInMinutes = 10,
   .dsmFzSetpointTemperatureInDegFx100 = 200,
   .defrostPeriodicTimeoutInSeconds = 1,
};

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 33 * MINUTES_PER_HOUR
};

#define CountLessThanFzAbnormalDefrostRunTimeInSeconds (defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 2)
#define CountGreaterThanFzAbnormalDefrostRunTimeInSeconds (defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1)
#define CountBetweenMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
#define CountGreaterThanMaxTimeBetweenDefrostsInSeconds (defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE + 1)

#define CountBetweenSabbathMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds (sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)
#define CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds (sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE + 1)

static const DefrostTimerCounterConfig_t config = {
   .defrostTimerCounterFsmStateErd = Erd_DefrostTimerCounterFsmState,
   .defrostTimerCounterRequestErd = Erd_DefrostTimerCounterRequest,
   .defrostTimerCountInSecondsErd = Erd_DefrostTimerCountInSeconds,
   .doorAccelerationRequestErd = Erd_DoorAccelerationRequest,
   .ffDoorAccelerationCountsErd = Erd_DefrostFfDoorAccelerationCount,
   .fzDoorAccelerationCountsErd = Erd_DefrostFzDoorAccelerationCount,
   .defrostTimerIsSatisfiedMonitorRequestErd = Erd_DefrostTimerIsSatisfiedMonitorRequest,
   .compressorResolvedSpeedErd = Erd_CompressorSpeed_ResolvedVote,
   .compressorSpeedConfigErd = Erd_CompressorSpeedConfig,
   .sabbathModeErd = Erd_SabbathMode,
   .fzDefrostWasAbnormalErd = Erd_FzDefrostWasAbnormal,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes,
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

   void DefrostTimerCountShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostTimerCountInSeconds, &actualCount);

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

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void FzDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFzDoorAccelerationCount, &count);
   }

   void FfDoorAccelerationIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostFfDoorAccelerationCount, &count);
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

TEST(DefrostTimerCounter, ShouldResetDefrostTimerCountToZeroOnInit)
{
   Given DefrostTimerCountIs(999);

   When DefrostTimerCounterIsInitialized();
   DefrostTimerCountShouldBe(0);
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

TEST(DefrostTimerCounter, ShouldResetDefrostTimerCountToZeroWhenDisabled)
{
   Given DefrostTimerIsEnabled();

   When DefrostTimerCountIs(999);
   And DefrostTimerCounterRequestIs(DefrostTimer_Disable, 2);

   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);
   DefrostTimerCountShouldBe(0);
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

   When DefrostTimerCountIs(999);
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
TEST(DefrostTimerCounter, ShouldAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasNormalAndDefrostTimerCountIsBetweenFzAbnormalRunTimeAndMaxTimeBetweenDefrosts)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountBetweenMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountBetweenMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountBetweenMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds + 2 * SomeDoorAccelerationCounts);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasNormalAndDefrostTimerCountIsEqualToMaxTimeBetweenDefrosts)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(defrostData.maxTimeBetweenDefrostsInMinutes);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(defrostData.maxTimeBetweenDefrostsInMinutes);

   After(1);
   DefrostTimerCountShouldBe(defrostData.maxTimeBetweenDefrostsInMinutes);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasNormalAndDefrostTimerCountIsGreaterThanMaxTimeBetweenDefrosts)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasAbnormalAndDefrostTimerCountIsGreaterThanFzAbnormalDefrostRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasAbnormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasNormalAndDefrostTimerCountIsLessThanFzAbnormalDefrostRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountLessThanFzAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountLessThanFzAbnormalDefrostRunTimeInSeconds);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFzDefrostWasNormalAndDefrostTimerCountIsEqualToFzAbnormalRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFzDefrostWasNormalAndDefrostTimerCountIsEqualToMaxTimeBetweenDefrost)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(defrostData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFzDefrostWasNormalAndDefrostTimerCountIsGreaterThanMaxTimeBetweenDefrost)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountGreaterThanMaxTimeBetweenDefrostsInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldNotSendResetDoorAccelerationRequestWhenLastFzDefrostWasAbnormalAndDefrostTimerCountIsGreaterThanFzAbnormalRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasAbnormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}

TEST(DefrostTimerCounter, ShouldNotSendResetDoorAccelerationRequestWhenDefrostTimerCounterIsLessThanFzAbnormalRunTime)
{
   Given SabbathModeIs(DISABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}

// Sabbath mode enabled
TEST(DefrostTimerCounter, ShouldAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasNormalAndDefrostTimerCountIsBetweenFzAbnormalRunTimeAndSabbathMaxTimeBetweenDefrostsWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountBetweenSabbathMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountBetweenSabbathMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountBetweenSabbathMaxTimeBetweenDefrostsAndFzAbnormalDefrostRunTimeInSeconds + 2 * SomeDoorAccelerationCounts + 1);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasNormalAndDefrostTimerCountIsEqualToSabbathMaxTimeBetweenDefrostsWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(sabbathData.maxTimeBetweenDefrostsInMinutes);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(sabbathData.maxTimeBetweenDefrostsInMinutes);

   After(1);
   DefrostTimerCountShouldBe(sabbathData.maxTimeBetweenDefrostsInMinutes + 1);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasNormalAndDefrostTimerCountIsGreaterThanSabbathMaxTimeBetweenDefrostsWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds + 1);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasAbnormalAndDefrostTimerCountIsGreaterThanFzAbnormalDefrostRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasAbnormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds + 1);
}

TEST(DefrostTimerCounter, ShouldNotAddDoorAccelerationsToDefrostTimerCountWhenLastFzDefrostWasNormalAndDefrostTimerCountIsLessThanFzAbnormalDefrostRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given FfDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given FzDoorAccelerationIs(SomeDoorAccelerationCounts);
   Given DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostTimerCountShouldBe(CountLessThanFzAbnormalDefrostRunTimeInSeconds);

   After(1);
   DefrostTimerCountShouldBe(CountLessThanFzAbnormalDefrostRunTimeInSeconds + 1);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFzDefrostWasNormalAndDefrostTimerCountIsEqualToFzAbnormalRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(defrostData.fzAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFzDefrostWasNormalAndDefrostTimerCountIsEqualToSabbathMaxTimeBetweenDefrostWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(sabbathData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldSendResetDoorAccelerationRequestWhenLastFzDefrostWasNormalAndDefrostTimerCountIsGreaterThanSabbathMaxTimeBetweenDefrostWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountGreaterThanSabbathMaxTimeBetweenDefrostsInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Reset, 3);
}

TEST(DefrostTimerCounter, ShouldNotSendResetDoorAccelerationRequestWhenLastFzDefrostWasAbnormalAndDefrostTimerCountIsGreaterThanFzAbnormalRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasAbnormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountGreaterThanFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}

TEST(DefrostTimerCounter, ShouldNotSendResetDoorAccelerationRequestWhenDefrostTimerCounterIsLessthanFzAbnormalRunTimeWhenSabbathIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   Given SingleSpeedCompressorIsOff();
   Given LastFzDefrostWasNormal();
   Given DefrostTimerIsEnabled();
   Given DefrostTimerCountIs(CountLessThanFzAbnormalDefrostRunTimeInSeconds);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);

   After(1);
   DoorAccelerationRequestShouldBe(DoorAcceleration_Enable, 2);
}
