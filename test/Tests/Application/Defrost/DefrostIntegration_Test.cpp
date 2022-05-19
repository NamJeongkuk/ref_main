/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Defrost.h"
#include "DefrostTimerCounter.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "ValvePosition.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "GridData.h"
#include "PeriodicNvUpdaterPlugin.h"
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
   SixGridLines = 6,
   TwoDimensional = 2,
};

enum
{
   SomeDefrostCount = 99,
   SomeOtherDefrostCount = 500
};

#define PowerUpDelayInMs (5 * gridData.gridPeriodicRunRateInMSec)

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

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostDoorHoldoffRequestErd = Erd_DefrostDoorHoldOffRequest,
   .defrostTimerCounterRequestErd = Erd_DefrostTimerCounterRequest,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .defrostStateErd = Erd_DefrostState,
   .numberOfFreezerAbnormalDefrostCyclesErd = Erd_NumberofFreezerAbnormalDefrostCycles,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freezerAbnormalDefrostCycleCountErd = Erd_FreezerAbnormalDefrostCycleCount,
   .freezerDefrostCycleCountErd = Erd_FreezerDefrostCycleCount,
   .freshFoodDefrostHeaterDefrostVoteErd = Erd_FreshFoodDefrostHeater_DefrostVote,
   .freezerDefrostHeaterDefrostVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .defrostTimerCounterFsmStateErd = Erd_DefrostTimerCounterFsmState,
   .sealedSystemValvePositionErd = Erd_SealedSystemValvePosition,
   .defrostTimerIsSatisfiedErd = Erd_DefrostTimerIsSatisfied,
   .timerModuleErd = Erd_TimerModule
};

static const DefrostData_t defrostData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .freezerAbnormalRunTimeInMinutes = 6 * 60,
   .maxTimeBetweenDefrostsInMinutes = 32 * 60,
   .prechillFreezerSetpointInDegFx100 = -600,
   .prechillFreshFoodSetpointInDegFx100 = 4600,
   .prechillConvertibleCompartmentSetpointInDegFx100 = -600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
   .defrostDoorHoldoffTimeForFreshFoodAndFreezerInMinutes = 60,
   .defrostDoorHoldoffTimeForFreshFoodOnlyInMinutes = 50,
   .defrostMaxHoldoffTimeInMinutes = 60,
   .maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = 60,
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
   .maxTimeBetweenDefrostsInMinutes = 16 * MINUTES_PER_HOUR
};

static const GridData_t gridData = {
   .gridPeriodicRunRateInMSec = 1 * MSEC_PER_SEC
};

static TemperatureDegFx100_t freshFoodCalcAxisGridLines[] = { 0, -450, 150, 450, 950, 1150 };
static TemperatureDegFx100_t freezerCalcAxisGridLines[] = { -250, 0, 250, 600, 750, 5500 };

static const CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freshFoodCalcAxisGridLines
};

static const CalculatedAxisGridLines_t freezerCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freezerCalcAxisGridLines
};

static CalculatedAxisGridLines_t calcGrid[] = { freshFoodCalcAxis, freezerCalcAxis };

static const CalculatedGridLines_t calcGridLines = {
   .dimensions = TwoDimensional,
   .gridLines = calcGrid
};

static const SystemMonitorData_t systemMonitorData = {
   .periodicNvUpdateInMinutes = 60
};

TEST_GROUP(DefrostIntegration)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   Defrost_t defrostInstance;
   EventSubscription_t defrostTimerCounterRequestSubscription;
   DefrostTimerCounter_t defrostTimerCounterInstance;
   PeriodicNvUpdaterPlugin_t periodicNvUpdaterPluginInstance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetSabbath(&personalityParametricData, &sabbathData);
      PersonalityParametricData_TestDouble_SetGrid(&personalityParametricData, &gridData);
      PersonalityParametricData_TestDouble_SetSystemMonitor(&personalityParametricData, &systemMonitorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void PeriodicNvUpdaterPluginIsInitialized()
   {
      PeriodicNvUpdaterPlugin_Init(&periodicNvUpdaterPluginInstance, dataModel);
   }

   void DefrostTimerCounterIsInitialized()
   {
      DefrostTimerCounter_Init(&defrostTimerCounterInstance, dataModel, &config);
   }

   void DefrostIsInitialized()
   {
      Defrost_Init(&defrostInstance, dataModel, &defrostConfig);

      EventSubscription_Init(
         &defrostTimerCounterRequestSubscription,
         &defrostInstance,
         DefrostTimerCounterRequestSent);

      DataModel_Subscribe(
         dataModel,
         Erd_DefrostTimerCounterRequest,
         &defrostTimerCounterRequestSubscription);
   }

   static void DefrostTimerCounterRequestSent(void *context, const void *_args)
   {
      IGNORE(context);
      REINTERPRET(request, _args, const DefrostTimerCounterRequest_t *);

      mock()
         .actualCall("DefrostTimerCounterRequestSent")
         .withParameter("request->request", request->request)
         .withParameter("request->requestId", request->requestId);
   }

   void DefrostTimerCounterRequestShouldBeWrittenWith(DefrostTimerCounterRequest_t request)
   {
      mock()
         .expectOneCall("DefrostTimerCounterRequestSent")
         .withParameter("request->request", request.request)
         .withParameter("request->requestId", request.requestId);
   }

   void DefrostInitializedWithFreezerTempAboveExtremeHysteresis()
   {
      Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh] + 1);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostInitializedWithFreezerTempBelowExtremeHysteresis()
   {
      Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh] - 1);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostTimerRequestShouldBe(DefrostTimer_t expectedRequest, Signal_t expectedId)
   {
      DefrostTimerCounterRequest_t actualRequest;
      DataModel_Read(dataModel, Erd_DefrostTimerCounterRequest, &actualRequest);

      CHECK_EQUAL(expectedRequest, actualRequest.request);
      CHECK_EQUAL(expectedId, actualRequest.requestId);
   }

   void EnableThenResetDefrostTimerCounterRequestsShouldBeSent()
   {
      DefrostTimerCounterRequest_t enableRequest;
      enableRequest.request = DefrostTimer_Enable;
      enableRequest.requestId = 1;

      DefrostTimerCounterRequestShouldBeWrittenWith(enableRequest);

      DefrostTimerCounterRequest_t resetRequest;
      resetRequest.request = DefrostTimer_Reset;
      resetRequest.requestId = 2;

      DefrostTimerCounterRequestShouldBeWrittenWith(resetRequest);
   }

   void EnableDefrostTimerCounterRequestShouldBeSent()
   {
      DefrostTimerCounterRequest_t enableRequest;
      enableRequest.request = DefrostTimer_Enable;
      enableRequest.requestId = 1;
      DefrostTimerCounterRequestShouldBeWrittenWith(enableRequest);
   }

   void DefrostTimerRequestIs(DefrostTimer_t request, Signal_t id)
   {
      DefrostTimerCounterRequest_t defrostTimerRequest;
      defrostTimerRequest.request = request;
      defrostTimerRequest.requestId = id;
      DataModel_Write(dataModel, Erd_DefrostTimerCounterRequest, &defrostTimerRequest);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolved, &temperature);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_t expectedState)
   {
      DefrostTimerCounterFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostTimerCounterFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DefrostTimerCountShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_DefrostTimerCountInSeconds, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void DefrostTimerCountIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_DefrostTimerCountInSeconds, &count);
   }

   void EepromDefrostTimerCountShouldBe(uint32_t expectedCount)
   {
      uint32_t actualCount;
      DataModel_Read(dataModel, Erd_Eeprom_DefrostTimerCountInSeconds, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void EepromDefrostTimerCountIs(uint32_t count)
   {
      DataModel_Write(dataModel, Erd_Eeprom_DefrostTimerCountInSeconds, &count);
   }

   void DefrostIsInPowerUpAndDefrostTimerCounterIsDisabledWithFreezerTempAboveExtremeHysteresis()
   {
      Given DefrostTimerCounterIsInitialized();
      And DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

      DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);
      DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);
   }

   void DefrostIsInPowerUpAndDefrostTimerCounterIsDisabledWithFreezerTempBelowExtremeHysteresis()
   {
      Given DefrostTimerCounterIsInitialized();
      And DefrostInitializedWithFreezerTempBelowExtremeHysteresis();

      DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);
      DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);
   }
};

TEST(DefrostIntegration, ShouldEnableDefrostTimerCounterAfterPowerUp)
{
   Given DefrostIsInPowerUpAndDefrostTimerCounterIsDisabledWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Disabled);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Enabled);
}

TEST(DefrostIntegration, ShouldResetBothDefrostTimerCountsToZeroWhenFreezerTempIsAboveExtremeHysteresisOnPowerUp)
{
   Given EepromDefrostTimerCountIs(SomeDefrostCount);
   And DefrostTimerCountIs(SomeOtherDefrostCount);
   And PeriodicNvUpdaterPluginIsInitialized();
   And DefrostIsInPowerUpAndDefrostTimerCounterIsDisabledWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Enabled);

   DefrostTimerCountShouldBe(0);
   EepromDefrostTimerCountShouldBe(0);
}

TEST(DefrostIntegration, ShouldCopyEepromDefrostTimerCountToRamOnPowerUpAndKeepValuesTheSameWhenDefrostTimerCounterIsInitializedAndFreezerTempIsBelowExtremeHysteresis)
{
   Given EepromDefrostTimerCountIs(SomeDefrostCount);
   And DefrostTimerCountIs(SomeOtherDefrostCount);
   And DefrostIsInPowerUpAndDefrostTimerCounterIsDisabledWithFreezerTempBelowExtremeHysteresis();
   And PeriodicNvUpdaterPluginIsInitialized();

   DefrostTimerCountShouldBe(SomeDefrostCount);
   EepromDefrostTimerCountShouldBe(SomeDefrostCount);

   After(PowerUpDelayInMs - 1);

   EnableDefrostTimerCounterRequestShouldBeSent();
   After(1);

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTimerCounterFsmStateShouldBe(DefrostTimerCounterFsmState_Enabled);

   DefrostTimerCountShouldBe(SomeDefrostCount);
   EepromDefrostTimerCountShouldBe(SomeDefrostCount);
}
