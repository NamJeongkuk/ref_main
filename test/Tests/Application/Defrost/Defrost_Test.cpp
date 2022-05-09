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
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "GridData.h"
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
   INVALID = false,
   VALID = true
};

enum
{
   TemperatureLessThanPrechillFreezerSetpoint = -700,
   TemperatureGreaterThanPrechillFreezerSetpoint = -500,
};

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostDoorHoldoffRequestErd = Erd_DefrostDoorHoldOffRequest,
   .defrostTimerCounterRequestErd = Erd_DefrostTimerCounterRequest,
   .freezerFilteredTemperatureErd = Erd_Freezer_FilteredTemperature,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .defrostStateErd = Erd_DefrostState,
   .numberOfFzAbnormalDefrostCyclesErd = Erd_NumberofFzAbnormalDefrostCycles,
   .fzDefrostWasAbnormalErd = Erd_FzDefrostWasAbnormal,
   .fzAbnormalDefrostCycleCountErd = Erd_FzAbnormalDefrostCycleCount,
   .fzDefrostCycleCountErd = Erd_FzDefrostCycleCount,
   .ffDefrostHeaterDefrostVoteErd = Erd_FfDefrostHeater_DefrostVote,
   .fzDefrostHeaterDefrostVoteErd = Erd_FzDefrostHeater_DefrostVote,
   .defrostTimerCounterFsmStateErd = Erd_DefrostTimerCounterFsmState,
   .sealedSystemValvePositionErd = Erd_SealedSystemValvePosition,
   .defrostTimerIsSatisfiedErd = Erd_DefrostTimerIsSatisfied,
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .iceCabinetFanDefrostVoteErd = Erd_IceCabinetFanSpeed_DefrostVote,
   .freezerSetpointDefrostVoteErd = Erd_FzSetpoint_DefrostVote,
   .freezerResolvedSetpointErd = Erd_FzSetpoint_ResolvedVote,
   .sealedSystemValvePositionDefrostVoteErd = Erd_ValvePosition_DefrostVote,
   .defrostIsFreshFoodOnlyErd = Erd_DefrostIsFreshFoodOnly,
   .freshFoodSetpointDefrostVoteErd = Erd_FfSetpoint_DefrostVote,
   .extendDefrostSignalErd = Erd_ExtendDefrostSignal,
   .compressorStateErd = Erd_CompressorState,
   .timerModuleErd = Erd_TimerModule
};

static const DefrostData_t defrostData = {
   .fzDoorIncrementFactorInSecondsPerSecond = 348,
   .ffDoorIncrementFactorInSecondsPerSecond = 87,
   .fzAbnormalRunTimeInMinutes = 6 * 60,
   .maxTimeBetweenDefrostsInMinutes = 32 * 60,
   .dmFzDefrostTemperatureInDegFx100 = 1500,
   .prechillFzSetpointInDegFx100 = -600,
   .prechillFfSetpointInDegFx100 = 4600,
   .prechillCcSetpointInDegFx100 = -600,
   .prechillFzEvapExitTemperatureInDegFx100 = -3000,
   .prechillCcEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
   .defrostDoorHoldoffTimeForFfAndFzInMinutes = 60,
   .defrostDoorHoldoffTimeForFfOnlyInMinutes = 50,
   .defrostMaxHoldoffTimeInMinutes = 60,
   .maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = 60,
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

static const DefrostData_t defrostDataWithZeroMaxPrechillHoldoffTime = {
   .fzDoorIncrementFactorInSecondsPerSecond = 348,
   .ffDoorIncrementFactorInSecondsPerSecond = 87,
   .fzAbnormalRunTimeInMinutes = 6 * 60,
   .maxTimeBetweenDefrostsInMinutes = 32 * 60,
   .dmFzDefrostTemperatureInDegFx100 = 1500,
   .prechillFzSetpointInDegFx100 = -600,
   .prechillFfSetpointInDegFx100 = 4600,
   .prechillCcSetpointInDegFx100 = -600,
   .prechillFzEvapExitTemperatureInDegFx100 = -3000,
   .prechillCcEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
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
   .maxTimeBetweenDefrostsInMinutes = 16 * MINUTES_PER_HOUR
};

static const GridData_t gridData = {
   .gridPeriodicRunRateInMSec = 1 * MSEC_PER_SEC
};

static const EvaporatorData_t singleEvaporatorData = {
   .numberOfEvaporators = 1
};

static const EvaporatorData_t dualEvaporatorData = {
   .numberOfEvaporators = 2
};

#define PowerUpDelayInMs 5 * gridData.gridPeriodicRunRateInMSec

enum
{
   SixGridLines = 6,
   TwoDimensional = 2,
};

static TemperatureDegFx100_t ffCalcAxisGridLines[] = { 0, -450, 150, 450, 950, 1150 };
static TemperatureDegFx100_t fzCalcAxisGridLines[] = { -250, 0, 250, 600, 750, 5500 };

static CalculatedAxisGridLines_t ffCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = ffCalcAxisGridLines
};

static CalculatedAxisGridLines_t fzCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = fzCalcAxisGridLines
};

static CalculatedAxisGridLines_t calcGrid[] = { ffCalcAxis, fzCalcAxis };

static CalculatedGridLines_t calcGridLines = {
   .dimensions = TwoDimensional,
   .gridLines = calcGrid
};

TEST_GROUP(Defrost_SingleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   Defrost_t instance;
   EventSubscription_t defrostTimerCounterRequestSubscription;

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
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &singleEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DefrostIsInitialized()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig);

      EventSubscription_Init(
         &defrostTimerCounterRequestSubscription,
         &instance,
         DefrostTimerCounterRequestSent);

      DataModel_Subscribe(
         dataModel,
         Erd_DefrostTimerCounterRequest,
         &defrostTimerCounterRequestSubscription);
   }

   void DefrostParametricSetWithMaxPrechillHoldoffOfZero()
   {
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostDataWithZeroMaxPrechillHoldoffTime);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DoorHoldoffRequestShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_DefrostDoorHoldOffRequest, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DefrostTimerRequestShouldBe(DefrostTimer_t expectedRequest, Signal_t expectedId)
   {
      DefrostTimerCounterRequest_t actualRequest;
      DataModel_Read(dataModel, Erd_DefrostTimerCounterRequest, &actualRequest);

      CHECK_EQUAL(expectedRequest, actualRequest.request);
      CHECK_EQUAL(expectedId, actualRequest.requestId);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperature, &temperature);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void FzAbnormalDefrostCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberofFzAbnormalDefrostCycles, &count);
   }

   void FzAbnormalDefrostCountShouldBe(uint16_t expectedCount)
   {
      uint16_t actualCount;
      DataModel_Read(dataModel, Erd_NumberofFzAbnormalDefrostCycles, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
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

   void LastFzDefrostShouldBeAbnormal()
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FzDefrostWasAbnormal, &actualState);

      CHECK_EQUAL(true, actualState);
   }

   void LastAbnormalFzDefrostCycleWas(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_FzAbnormalDefrostCycleCount, &count);
   }

   void LastAbnormalFzDefrostCycleShouldBe(uint16_t expectedCount)
   {
      uint16_t actualCount;
      DataModel_Read(dataModel, Erd_FzAbnormalDefrostCycleCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void CurrentDefrostCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_FzDefrostCycleCount, &count);
   }

   void DefrostInitializedWithFzTempAboveExtremeHysteresis()
   {
      Given FilteredFreezerCabinetTemperatureIs(fzCalcAxisGridLines[GridLine_FzExtremeHigh] + 1);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostInitializedWithFzTempAboveTerminationTemp()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.fzDefrostTerminationTemperatureInDegFx100 + 1);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostInitializedWithFzTempEqualToTerminationTemp()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.fzDefrostTerminationTemperatureInDegFx100);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given LastFzDefrostWasNormal();
      Given DefrostIsInitialized();
   }

   void LastFzDefrostWasAbnormalButCurrentFreezerCabinetTemperatureIsNotAbnormal()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given LastFzDefrostWasAbnormal();
      Given DefrostIsInitialized();
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      switch(state)
      {
         case DefrostHsmState_Idle:
            Given DefrostStateIs(DefrostState_Idle);
            Given LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature();

            After(PowerUpDelayInMs - 1);
            DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

            EnableDefrostTimerCounterRequestShouldBeSent();
            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

            When DefrostTimerIsSatisfiedIs(true);
            And SealedSystemValveIsInPosition(ValvePosition_C);

            After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
            SealedSystemValveIsInPosition(defrostData.threeWayValvePositionToExitIdle);

            DisableDefrostTimerCounterRequestShouldBeSent();
            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         case DefrostHsmState_PostPrechill:
            Given DefrostStateIs(DefrostState_Prechill);
            Given LastFzDefrostWasAbnormalButCurrentFreezerCabinetTemperatureIsNotAbnormal();

            After(PowerUpDelayInMs - 1);
            DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
            break;

         default:
            break;
      }
   }

   void FreezerEvaporatorThermistorIsInvalidAndPrechillIsSkippedAndInHeaterOnEntryState()
   {
      Given FreezerEvaporatorThermistorValidityIs(INVALID);
      Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
      When DefrostTimerIsSatisfiedIs(true);
      And SealedSystemValveIsInPosition(ValvePosition_C);

      After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC);
      SealedSystemValveIsInPosition(defrostData.threeWayValvePositionToExitIdle);

      After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);

      DisableDefrostTimerCounterRequestShouldBeSent();
      After(1);
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   }

   void FfDefrostHeaterVoteShouldBe(bool expectedState)
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FfDefrostHeater_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_TRUE(actualVote.care);
   }

   void FzDefrostHeaterVoteShouldBe(bool expectedState)
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FzDefrostHeater_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_TRUE(actualVote.care);
   }

   void DefrostTimerCounterIs(bool state)
   {
      DefrostTimerCounterFsmState_t fsmState = state ? DefrostTimerCounterFsmState_Enabled : DefrostTimerCounterFsmState_Disabled;
      DataModel_Write(dataModel, Erd_DefrostTimerCounterFsmState, &fsmState);
   }

   void DefrostTimerCounterRequestShouldBe(DefrostTimer_t expectedRequest, Signal_t expectedRequestId)
   {
      DefrostTimerCounterRequest_t actualRequest;
      DataModel_Read(dataModel, Erd_DefrostTimerCounterRequest, &actualRequest);

      CHECK_EQUAL(expectedRequest, actualRequest.request);
      CHECK_EQUAL(expectedRequestId, actualRequest.requestId);
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

   void EnableDefrostTimerCounterRequestShouldBeSent()
   {
      DefrostTimerCounterRequest_t enableRequest;
      enableRequest.request = DefrostTimer_Enable;
      enableRequest.requestId = 1;

      DefrostTimerCounterRequestShouldBeWrittenWith(enableRequest);
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

   void DisableDefrostTimerCounterRequestShouldBeSent()
   {
      DefrostTimerCounterRequest_t disableRequest;
      disableRequest.request = DefrostTimer_Disable;
      disableRequest.requestId = 2;

      DefrostTimerCounterRequestShouldBeWrittenWith(disableRequest);
   }

   void NothingShouldHappen()
   {
   }

   void DefrostTimerIsSatisfiedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostTimerIsSatisfied, &state);
   }

   void SealedSystemValveIsInPosition(ValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValvePosition, &position);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorIsValid, &state);
   }

   void NumberOfFreshFoodDefrostsBeforeAFreezerDefrostIs(uint8_t number)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost, &number);
   }

   void NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(uint8_t expectedNumber)
   {
      uint8_t actualNumber;
      DataModel_Read(dataModel, Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost, &actualNumber);

      CHECK_EQUAL(expectedNumber, actualNumber);
   }

   void FreshFoodHeaterShouldBeVoted(bool expectedState)
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FfDefrostHeater_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerHeaterShouldBeVoted(bool expectedState)
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FzDefrostHeater_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_TRUE(actualVote.care);
   }

   void IceCabinetFanShouldBeVoted(FanSpeed_t expectedSpeed)
   {
      FanVotedSpeed_t actualVote;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedSpeed, actualVote.speed);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerSetPointShouldBeVotedWith(TemperatureDegFx100_t expectedTemperature)
   {
      SetpointVotedTemperature_t actualVote;
      DataModel_Read(dataModel, Erd_FzSetpoint_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedTemperature, actualVote.temperature);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerResolvedSetpointIs(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t vote;
      vote.temperature = temperature;
      vote.care = true;
      DataModel_Write(dataModel, Erd_FzSetpoint_ResolvedVote, &vote);
   }

   void ValvePositionShouldBeVotedWith(ValvePosition_t expectedPosition)
   {
      ValveVotedPosition_t vote;
      DataModel_Read(dataModel, Erd_ValvePosition_DefrostVote, &vote);

      CHECK_EQUAL(expectedPosition, vote.position);
      CHECK_TRUE(vote.care);
   }

   void ValvePositionShouldNotBeVotedFor()
   {
      ValveVotedPosition_t vote;
      DataModel_Read(dataModel, Erd_ValvePosition_DefrostVote, &vote);

      CHECK_FALSE(vote.care);
   }

   void CurrentDefrostIsFreshFoodOnly()
   {
      bool currentDefrostIsFreshFoodOnly = true;
      DataModel_Write(dataModel, Erd_DefrostIsFreshFoodOnly, &currentDefrostIsFreshFoodOnly);
   }

   void CurrentDefrostIsNotFreshFoodOnly()
   {
      bool currentDefrostIsFreshFoodOnly = false;
      DataModel_Write(dataModel, Erd_DefrostIsFreshFoodOnly, &currentDefrostIsFreshFoodOnly);
   }

   void FreshFoodSetpointShouldBeVotedWith(TemperatureDegFx100_t expectedTemperature)
   {
      SetpointVotedTemperature_t actualVote;
      DataModel_Read(dataModel, Erd_FfSetpoint_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedTemperature, actualVote.temperature);
      CHECK_TRUE(actualVote.care);
   }

   void FreshFoodSetpointShouldNotBeVotedFor()
   {
      SetpointVotedTemperature_t vote;
      DataModel_Read(dataModel, Erd_FfSetpoint_DefrostVote, &vote);

      CHECK_FALSE(vote.care);
   }

   void ValveIsNotInPositionUponExitOfIdleAndOnEntryToPrechillPrep()
   {
      Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

      When DefrostTimerIsSatisfiedIs(true);
      And SealedSystemValveIsInPosition(ValvePosition_C);

      After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC + defrostData.maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds * MSEC_PER_SEC - 1); // this timing has to be flipped for the max time of 0 b/c can't do -1
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);

      DisableDefrostTimerCounterRequestShouldBeSent();
      After(1);
      DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   }

   void ResetExtendDefrostSignalToZero()
   {
      Signal_t signal = 0;
      DataModel_Write(dataModel, Erd_ExtendDefrostSignal, &signal);
   }

   void ExtendDefrostSignalShouldBeSent()
   {
      Signal_t signal;
      DataModel_Read(dataModel, Erd_ExtendDefrostSignal, &signal);
      CHECK_EQUAL(signal, 1);
   }

   void ExtendDefrostSignalShouldNotBeSent()
   {
      Signal_t signal;
      DataModel_Read(dataModel, Erd_ExtendDefrostSignal, &signal);
      CHECK_EQUAL(signal, 0);
   }

   void CompressorStateTimeIsSatisfied()
   {
      CompressorState_t state = CompressorState_On;
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }
};

TEST(Defrost_SingleEvap, ShouldInitializeIntoPowerUpHsmState)
{
   Given CalculatedGridLinesAre(calcGridLines);
   Given DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);
}

TEST(Defrost_SingleEvap, ShouldRequestEnableThenResetOfDefrostTimerCounterWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAfterPowerUpDelay)
{
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostTimerRequestShouldBe(DefrostTimer_Reset, 0);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
}

TEST(Defrost_SingleEvap, ShouldRequestEnableThenResetOfDefrostTimerWhenFilteredFzCabinetTemperatureIsGreaterThanFzDefrostTerminationTemperatureAfterPowerUpDelay)
{
   Given DefrostInitializedWithFzTempAboveTerminationTemp();

   After(PowerUpDelayInMs - 1);
   DefrostTimerRequestShouldBe(DefrostTimer_Reset, 0);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
}

TEST(Defrost_SingleEvap, ShouldRequestEnableThenResetOfDefrostTimerWhenFilteredFzCabinetTemperatureIsEqualToFzDefrostTerminationTemperatureAfterPowerUpDelay)
{
   Given DefrostInitializedWithFzTempEqualToTerminationTemp();

   After(PowerUpDelayInMs - 1);
   DefrostTimerRequestShouldBe(DefrostTimer_Reset, 0);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
}

TEST(Defrost_SingleEvap, ShouldGoToDwellWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateWasHeaterOnAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_HeaterOn);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsPrechillAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Prechill);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsIdleAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Idle);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsDwellAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Dwell);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsDisabledAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldIncrementFzAbnormalDefrostCountWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFzDefrostWasNormalAndLastFzDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FzAbnormalDefrostCountIs(0);
   Given LastFzDefrostWasNormal();
   Given LastAbnormalFzDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   FzAbnormalDefrostCountShouldBe(0);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   FzAbnormalDefrostCountShouldBe(1);
}

TEST(Defrost_SingleEvap, ShouldSetLastFzDefrostAsAbnormalWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFzDefrostWasNormalAndLastFzDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FzAbnormalDefrostCountIs(0);
   Given LastFzDefrostWasNormal();
   Given LastAbnormalFzDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   LastFzDefrostWasNormal();

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   LastFzDefrostShouldBeAbnormal();
}

TEST(Defrost_SingleEvap, ShouldSaveLastFzAbnormalDefrostCountWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFzDefrostWasNormalAndLastFzDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FzAbnormalDefrostCountIs(0);
   Given LastFzDefrostWasNormal();
   Given LastAbnormalFzDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   LastAbnormalFzDefrostCycleShouldBe(32);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   LastAbnormalFzDefrostCycleShouldBe(35);
}

TEST(Defrost_SingleEvap, ShouldSetLastFzDefrostAsAbnormalWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFzDefrostWasNormalAndLastFzDefrostCycleCountThatWasAbnormalEqualsCurrentDefrostCountAfterPowerUpDelay)
{
   Given FzAbnormalDefrostCountIs(0);
   Given LastFzDefrostWasNormal();
   Given LastAbnormalFzDefrostCycleWas(35);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   LastFzDefrostWasNormal();

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   LastFzDefrostShouldBeAbnormal();
}

TEST(Defrost_SingleEvap, ShouldSaveLastFzAbnormalDefrostCountWhenFilteredFzCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFzDefrostWasAbnormalAndLastFzDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FzAbnormalDefrostCountIs(0);
   Given LastFzDefrostWasAbnormal();
   Given LastAbnormalFzDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFzTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   LastAbnormalFzDefrostCycleShouldBe(32);

   EnableThenResetDefrostTimerCounterRequestsShouldBeSent();
   After(1);
   LastAbnormalFzDefrostCycleShouldBe(35);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenLastFzDefrostWasNormalAndDefrostStateWasPrechillAfterPowerUpDelay)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostStateIs(DefrostState_Prechill);
   Given LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature();
   Given DefrostIsInitialized();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPostPrechillWhenLastFzDefrostWasAbnormalAndDefrostStateWasPrechillAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Prechill);
   Given LastFzDefrostWasAbnormalButCurrentFreezerCabinetTemperatureIsNotAbnormal();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnWhenLastFzDefrostWasNormalAndDefrostStateWasHeaterOnAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_HeaterOn);
   Given LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature();
   Given DefrostIsInitialized();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenLastFzDefrostWasNormalAndDefrostStateWasDwellAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Dwell);
   Given LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature();
   Given DefrostIsInitialized();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   EnableDefrostTimerCounterRequestShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenLastFzDefrostWasNormalAndDefrostStateWasDisabledAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature();
   Given DefrostIsInitialized();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   EnableDefrostTimerCounterRequestShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleAndSendEnableDefrostTimerCounterRequestWhenLastFzDefrostWasNormalAndDefrostStateWasIdleAfterPowerUpDelay)
{
   Given DefrostTimerCounterIs(DISABLED);
   And DefrostStateIs(DefrostState_Idle);
   And LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature();
   And DefrostIsInitialized();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   EnableDefrostTimerCounterRequestShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldVoteForFfAndFzDefrostHeatersOffWhenEnteringIdle)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   FfDefrostHeaterVoteShouldBe(OFF);
   FzDefrostHeaterVoteShouldBe(OFF);
}

TEST(Defrost_SingleEvap, ShouldNotSendEnableDefrostTimerCounterRequestWhenDefrostTimerCountingIsEnabled)
{
   Given DefrostTimerCounterIs(ENABLED);
   And DefrostStateIs(DefrostState_Idle);
   And LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature();
   And DefrostIsInitialized();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   NothingShouldHappen();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenDefrostTimerIsSatisfiedAndSealedSystemValveIsInParametricDefinedPositionAfterPeriodicTimeout)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTimerIsSatisfiedIs(true);
   And SealedSystemValveIsInPosition(defrostData.threeWayValvePositionToExitIdle);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   DisableDefrostTimerCounterRequestShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenDefrostTimerIsSatisfiedAndSealedSystemValveIsNeverInParametricDefinedPositionAfterPeriodicTimeoutAndMaxPrechillHoldoffTime)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTimerIsSatisfiedIs(true);
   And SealedSystemValveIsInPosition(ValvePosition_C);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC + defrostData.maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds * MSEC_PER_SEC - 1); // this timing has to be flipped for the max time of 0 b/c can't do -1
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   DisableDefrostTimerCounterRequestShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenDefrostTimerIsSatisfiedAndSealedSystemValveMovesIntoParametricDefinedPositionBeforeTheMaxPrechillHoldoffTimeExpires)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTimerIsSatisfiedIs(true);
   And SealedSystemValveIsInPosition(ValvePosition_C);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC);
   SealedSystemValveIsInPosition(defrostData.threeWayValvePositionToExitIdle);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   DisableDefrostTimerCounterRequestShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenDefrostTimerIsSatisfiedAndSealedSystemValveIsNotInParametricDefinedPositionWhenMaxPrechillHoldoffTimeIsZero)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostParametricSetWithMaxPrechillHoldoffOfZero();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTimerIsSatisfiedIs(true);
   And SealedSystemValveIsInPosition(ValvePosition_C);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   DisableDefrostTimerCounterRequestShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenFreezerEvapThermistorIsInvalidOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(INVALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   When DefrostTimerIsSatisfiedIs(true);
   And SealedSystemValveIsInPosition(ValvePosition_C);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC);
   SealedSystemValveIsInPosition(defrostData.threeWayValvePositionToExitIdle);

   After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   DisableDefrostTimerCounterRequestShouldBeSent();
   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidDuringPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(INVALID);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidDuringPrechill)
{
   // TBD
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidDuringPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   When FreezerEvaporatorThermistorValidityIs(INVALID);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldIncrementNumberOfFreshFoodDefrostsBeforeAFreezerDefrostOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(1);
}

TEST(Defrost_SingleEvap, ShouldNotIncrementNumberOfFreshFoodDefrostsBeforeAFreezerDefrostOnEntryToPrechillPrepIfFreezerEvaporatorThermistorIsInvalid)
{
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostIs(0);
   Given FreezerEvaporatorThermistorIsInvalidAndPrechillIsSkippedAndInHeaterOnEntryState();

   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(0);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreshFoodHeaterOffOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreshFoodHeaterShouldBeVoted(OFF);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreezerHeaterOffOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreezerHeaterShouldBeVoted(OFF);
}

TEST(Defrost_SingleEvap, ShouldVoteForIceCabinetFanHighOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   IceCabinetFanShouldBeVoted(FanSpeed_High);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreezerSetPointWithResolvedFreezerSetpointOnEntryToPrechillPrepAndResolvedFreezerSetpointIsLessThanPrechillSetpoint)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given FreezerResolvedSetpointIs(TemperatureLessThanPrechillFreezerSetpoint);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreezerSetPointShouldBeVotedWith(TemperatureLessThanPrechillFreezerSetpoint);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreezerSetPointWithPrechillFreezerSetpointOnEntryToPrechillPrepAndResolvedFreezerSetpointIsEqualToPrechillSetpoint)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given FreezerResolvedSetpointIs(defrostData.prechillFzSetpointInDegFx100);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreezerSetPointShouldBeVotedWith(defrostData.prechillFzSetpointInDegFx100);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreezerSetPointWithPrechillFreezerSetpointOnEntryToPrechillPrepAndResolvedFreezerSetpointIsGreaterThanPrechillSetpoint)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given FreezerResolvedSetpointIs(TemperatureGreaterThanPrechillFreezerSetpoint);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreezerSetPointShouldBeVotedWith(defrostData.prechillFzSetpointInDegFx100);
}

TEST(Defrost_SingleEvap, ShouldVoteForValvePositionIfMaxPrechillHoldoffTimeIsGreaterThanZeroOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   ValvePositionShouldBeVotedWith(defrostData.threeWayValvePositionForMaxPrechillHoldoff);
}

TEST(Defrost_SingleEvap, ShouldNotVoteForValvePositionIfMaxPrechillHoldoffTimeIsEqualToZeroOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostParametricSetWithMaxPrechillHoldoffOfZero();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   ValvePositionShouldNotBeVotedFor();
}

TEST(Defrost_SingleEvap, ShouldVoteForFreshFoodSetpointIfCurrentDefrostIsNotFreshFoodOnlyOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given CurrentDefrostIsNotFreshFoodOnly();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreshFoodSetpointShouldBeVotedWith(defrostData.prechillFfSetpointInDegFx100);
}

TEST(Defrost_SingleEvap, ShouldNotVoteForFreshFoodSetpointIfCurrentDefrostIsFreshFoodOnlyOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given CurrentDefrostIsFreshFoodOnly();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreshFoodSetpointShouldNotBeVotedFor();
}

TEST(Defrost_SingleEvap, ShouldRequestToExtendDefrostWithFreshFoodCycleDefrostIfCurrentValvePositionIsInPositionToExtendDefrostOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given SealedSystemValveIsInPosition(defrostData.threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   ExtendDefrostSignalShouldBeSent();
}

TEST(Defrost_SingleEvap, ShouldNotRequestToExtendDefrostWithFreshFoodCycleDefrostIfCurrentValvePositionIsNotInPositionToExtendDefrostOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given ValveIsNotInPositionUponExitOfIdleAndOnEntryToPrechillPrep();

   ExtendDefrostSignalShouldNotBeSent();
}

TEST(Defrost_SingleEvap, ShouldRequestToEnableDoorHoldoffOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   DoorHoldoffRequestShouldBe(enabled);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostPrechillFromPrechillPrepWhenLastDefrostChangesFromNormalToAbnormal)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When LastFzDefrostWasAbnormal();
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillFromPrechillPrepWhenCompressorStateTimeIsSatisfied)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldNotTransitionFromPrechillPrepWhenCompressorStateIsMinimumOffTime)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateIs(CompressorState_MinimumOffTime);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldNotTransitionFromPrechillPrepWhenCompressorStateIsMinimumOnTime)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldNotTransitionFromPrechillPrepWhenCompressorStateIsMinimumRunTime)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateIs(CompressorState_MinimumRunTime);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST_GROUP(Defrost_DualEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   Defrost_t instance;
   EventSubscription_t defrostTimerCounterRequestSubscription;

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
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &dualEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DefrostIsInitialized()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig);

      EventSubscription_Init(
         &defrostTimerCounterRequestSubscription,
         &instance,
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

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperature, &temperature);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
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

   void LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given LastFzDefrostWasNormal();
      Given DefrostIsInitialized();
   }

   void LastFzDefrostWasAbnormalButCurrentFreezerCabinetTemperatureIsNotAbnormal()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given LastFzDefrostWasAbnormal();
      Given DefrostIsInitialized();
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      switch(state)
      {
         case DefrostHsmState_Idle:
            Given DefrostStateIs(DefrostState_Idle);
            Given LastFzDefrostWasNormalBasedOnAbnormalFreezerCabinetTemperature();

            After(PowerUpDelayInMs - 1);
            DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

            EnableDefrostTimerCounterRequestShouldBeSent();
            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

            When DefrostTimerIsSatisfiedIs(true);
            And SealedSystemValveIsInPosition(ValvePosition_C);

            After(defrostData.defrostPeriodicTimeoutInSeconds * MSEC_PER_SEC - 1);
            SealedSystemValveIsInPosition(defrostData.threeWayValvePositionToExitIdle);

            DisableDefrostTimerCounterRequestShouldBeSent();
            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;
      }
   }

   void EnableDefrostTimerCounterRequestShouldBeSent()
   {
      DefrostTimerCounterRequest_t enableRequest;
      enableRequest.request = DefrostTimer_Enable;
      enableRequest.requestId = 1;

      DefrostTimerCounterRequestShouldBeWrittenWith(enableRequest);
   }

   void DefrostTimerIsSatisfiedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostTimerIsSatisfied, &state);
   }

   void SealedSystemValveIsInPosition(ValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValvePosition, &position);
   }

   void DefrostTimerCounterRequestShouldBeWrittenWith(DefrostTimerCounterRequest_t request)
   {
      mock()
         .expectOneCall("DefrostTimerCounterRequestSent")
         .withParameter("request->request", request.request)
         .withParameter("request->requestId", request.requestId);
   }

   void DisableDefrostTimerCounterRequestShouldBeSent()
   {
      DefrostTimerCounterRequest_t disableRequest;
      disableRequest.request = DefrostTimer_Disable;
      disableRequest.requestId = 2;

      DefrostTimerCounterRequestShouldBeWrittenWith(disableRequest);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorIsValid, &state);
   }

   void FreezerSetPointShouldBeVotedWith(TemperatureDegFx100_t expectedTemperature)
   {
      SetpointVotedTemperature_t actualVote;
      DataModel_Read(dataModel, Erd_FzSetpoint_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedTemperature, actualVote.temperature);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerResolvedSetpointIs(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t vote;
      vote.temperature = temperature;
      vote.care = true;
      DataModel_Write(dataModel, Erd_FzSetpoint_ResolvedVote, &vote);
   }
};

TEST(Defrost_DualEvap, ShouldVoteForFreezerSetPointWithPrechillFreezerSetpointOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreezerSetPointShouldBeVotedWith(defrostData.prechillFzSetpointInDegFx100);
}
