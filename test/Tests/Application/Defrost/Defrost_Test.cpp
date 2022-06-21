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
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "GridData.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "DefrostData_TestDouble.h"
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
   SixGridLines = 6,
   TwoDimensional = 2,
};

enum
{
   DontCare = false,
   Care = true
};

enum
{
   TemperatureLessThanPrechillFreezerSetpoint = -700,
   TemperatureGreaterThanPrechillFreezerSetpoint = -500,
   SomeMaxPrechillTimeInMinutes = 5,
   SomeAdjustedFreezerSetpointDegFx100 = 100
};

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostDoorHoldoffRequestErd = Erd_DefrostDoorHoldOffRequest,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .defrostStateErd = Erd_DefrostState,
   .numberOfFreezerAbnormalDefrostCyclesErd = Erd_NumberofFreezerAbnormalDefrostCycles,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freezerAbnormalDefrostCycleCountErd = Erd_FreezerAbnormalDefrostCycleCount,
   .freezerDefrostCycleCountErd = Erd_FreezerDefrostCycleCount,
   .freshFoodDefrostHeaterDefrostVoteErd = Erd_FreshFoodDefrostHeater_DefrostVote,
   .freezerDefrostHeaterDefrostVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .sealedSystemValvePositionErd = Erd_SealedSystemValvePosition,
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .iceCabinetFanDefrostVoteErd = Erd_IceCabinetFanSpeed_DefrostVote,
   .freezerSetpointDefrostVoteErd = Erd_FreezerSetpoint_DefrostVote,
   .freezerResolvedSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .sealedSystemValvePositionDefrostVoteErd = Erd_ValvePosition_DefrostVote,
   .defrostIsFreshFoodOnlyErd = Erd_DefrostIsFreshFoodOnly,
   .freshFoodSetpointDefrostVoteErd = Erd_FreshFoodSetpoint_DefrostVote,
   .extendDefrostSignalErd = Erd_ExtendDefrostSignal,
   .compressorStateErd = Erd_CompressorState,
   .defrostMaxHoldoffMetErd = Erd_DefrostMaxHoldoffMet,
   .defrostPrechillRunCounterInMinutesErd = Erd_DefrostPrechillRunCounterInMinutes,
   .timeInMinutesInValvePositionBErd = Erd_TimeInMinutesInValvePositionB,
   .prechillTimeMetErd = Erd_PrechillTimeMet,
   .maxPrechillTimeInMinutesErd = Erd_MaxPrechillTimeInMinutes,
   .freezerEvaporatorFilteredTemperatureErd = Erd_FreezerEvap_FilteredTemperatureResolved,
   .adjustedFreezerSetpointErd = Erd_Freezer_AdjustedSetpoint,
   .doorHoldoffTimeIsSatisfiedErd = Erd_DefrostDoorHoldoffTimeSatisfied,
   .noFreezeLimitIsActiveErd = Erd_NoFreezeLimitIsActive,
   .freezerEvapFanDefrostVoteErd = Erd_FreezerEvapFanSpeed_DefrostVote,
   .freshFoodEvapFanDefrostVoteErd = Erd_FreshFoodEvapFanSpeed_DefrostVote,
   .timerModuleErd = Erd_TimerModule
};

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 16 * MINUTES_PER_HOUR
};

static const DeltaGridLineData_t freshFoodGridLineData[] = {
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b0010,
   },
   {
      .gridLinesDegFx100 = -450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 150,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 950,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 1150,
      .bitMapping = 0b1000,
   },
};

static const DeltaGridLineData_t freezerGridLineData[] = {
   {
      .gridLinesDegFx100 = -250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 600,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 750,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 5500,
      .bitMapping = 0b0010,
   },
};

static const DeltaAxisGridLines_t freshFoodAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freshFoodGridLineData
};

static const DeltaAxisGridLines_t freezerAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freezerGridLineData
};

static DeltaAxisGridLines_t parametricGrid[] = { freshFoodAxis, freezerAxis };
static DeltaGridLines_t deltaGrid = {
   .dimensions = TwoDimensional,
   .gridLines = parametricGrid
};

static const GridData_t gridData = {
   .gridId = 0,
   .deltaGridLines = &deltaGrid,
   .gridPeriodicRunRateInMSec = 1 * MSEC_PER_SEC
};

static const EvaporatorData_t singleEvaporatorData = {
   .numberOfEvaporators = 1
};

static const EvaporatorData_t dualEvaporatorData = {
   .numberOfEvaporators = 2
};

#define PowerUpDelayInMs 5 * gridData.gridPeriodicRunRateInMSec

static TemperatureDegFx100_t freshFoodCalcAxisGridLines[] = { 0, -450, 150, 450, 950, 1150 };
static TemperatureDegFx100_t freezerCalcAxisGridLines[] = { -250, 0, 250, 600, 750, 5500 };

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freshFoodCalcAxisGridLines
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freezerCalcAxisGridLines
};

static CalculatedAxisGridLines_t calcGrid[] = { freshFoodCalcAxis, freezerCalcAxis };

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
   DefrostData_t defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      DefrostData_TestDouble_Init(&defrostData);
      DefrostData_TestDouble_SetMaxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds(&defrostData, 60);

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
   }

   void DefrostParametricSetWithMaxPrechillHoldoffOfZero()
   {
      DefrostData_TestDouble_SetMaxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds(&defrostData, 0);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
   }

   void DefrostParametricSetWithThreeWayValveTimePriorToPrechillCountsAsPrechillTimeCleared()
   {
      DefrostData_TestDouble_SetThreeWayValveTimePriorToPrechillCountsAsPrechillTime(&defrostData, false);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
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

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolved, &temperature);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void FreezerAbnormalDefrostCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberofFreezerAbnormalDefrostCycles, &count);
   }

   void FreezerAbnormalDefrostCountShouldBe(uint16_t expectedCount)
   {
      uint16_t actualCount;
      DataModel_Read(dataModel, Erd_NumberofFreezerAbnormalDefrostCycles, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
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

   void LastFreezerDefrostShouldBeAbnormal()
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerDefrostWasAbnormal, &actualState);

      CHECK_EQUAL(true, actualState);
   }

   void LastAbnormalFreezerDefrostCycleWas(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_FreezerAbnormalDefrostCycleCount, &count);
   }

   void LastAbnormalFreezerDefrostCycleShouldBe(uint16_t expectedCount)
   {
      uint16_t actualCount;
      DataModel_Read(dataModel, Erd_FreezerAbnormalDefrostCycleCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void CurrentDefrostCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostCycleCount, &count);
   }

   void DefrostInitializedWithFreezerTempAboveExtremeHysteresis()
   {
      Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh] + 1);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostInitializedWithFreezerTempAboveTerminationTemp()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.freezerDefrostTerminationTemperatureInDegFx100 + 1);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostInitializedWithFreezerTempEqualToTerminationTemp()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.freezerDefrostTerminationTemperatureInDegFx100);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostInitializedWithNormalFreezerCabinetTemperatures()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void FreshFoodDefrostHeaterVoteIs(bool state, bool care)
   {
      HeaterVotedState_t vote;
      vote.state = state;
      vote.care = care;

      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &vote);
   }

   void FreezerDefrostHeaterVoteIs(bool state, bool care)
   {
      HeaterVotedState_t vote;
      vote.state = state;
      vote.care = care;

      DataModel_Write(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &vote);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      switch(state)
      {
         case DefrostHsmState_Idle:
            Given DefrostStateIs(DefrostState_Idle);
            Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

            After(PowerUpDelayInMs - 1);
            DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostStateIs(DefrostState_Prechill);
            Given FreezerEvaporatorThermistorValidityIs(VALID);
            Given CompressorStateIs(CompressorState_MinimumOnTime);
            Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

            After(PowerUpDelayInMs - 1);
            DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         case DefrostHsmState_Prechill:
            Given FreezerEvaporatorThermistorValidityIs(VALID);
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

            When CompressorStateTimeIsSatisfied();
            DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
            break;

         case DefrostHsmState_PostPrechill:
            Given DefrostStateIs(DefrostState_Prechill);
            Given CompressorStateIs(CompressorState_MinimumOnTime);
            Given DefrostInitializedWithNormalFreezerCabinetTemperatures();
            Given LastFreezerDefrostWasAbnormal();
            And FreshFoodDefrostHeaterVoteIs(ON, Care);
            And FreezerDefrostHeaterVoteIs(ON, Care);

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
      Given DefrostStateIs(DefrostState_Prechill);
      Given FreezerEvaporatorThermistorValidityIs(INVALID);
      Given CompressorStateIs(CompressorState_MinimumOnTime);
      Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

      After(PowerUpDelayInMs - 1);
      DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

      After(1);
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   }

   void FreshFoodDefrostHeaterVoteShouldBe(bool expectedState)
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerDefrostHeaterVoteShouldBe(bool expectedState)
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_TRUE(actualVote.care);
   }

   void NothingShouldHappen()
   {
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
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerHeaterShouldBeVoted(bool expectedState)
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actualVote);

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
      DataModel_Read(dataModel, Erd_FreezerSetpoint_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedTemperature, actualVote.temperature);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerResolvedSetpointIs(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t vote;
      vote.temperature = temperature;
      vote.care = true;
      DataModel_Write(dataModel, Erd_FreezerSetpoint_ResolvedVote, &vote);
   }

   void ValvePositionShouldBeVotedWith(ValvePosition_t expectedPosition)
   {
      ValveVotedPosition_t vote;
      DataModel_Read(dataModel, Erd_ValvePosition_DefrostVote, &vote);

      CHECK_EQUAL(expectedPosition, vote.position);
      CHECK_TRUE(vote.care);
   }

   void ValvePositionShouldBeDontCare()
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
      DataModel_Read(dataModel, Erd_FreshFoodSetpoint_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedTemperature, actualVote.temperature);
      CHECK_TRUE(actualVote.care);
   }

   void FreshFoodSetpointShouldNotBeVotedFor()
   {
      SetpointVotedTemperature_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodSetpoint_DefrostVote, &vote);

      CHECK_FALSE(vote.care);
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

   void CompressorStateShouldBe(CompressorState_t expectedState)
   {
      CompressorState_t actualState;
      DataModel_Read(dataModel, Erd_CompressorState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void PrechillRunCounterInMinutesShouldBe(uint16_t expectedMinutes)
   {
      uint16_t actualMinutes;
      DataModel_Read(dataModel, Erd_DefrostPrechillRunCounterInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void DefrostMaxHoldoffIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostMaxHoldoffMet, &state);
   }

   void DefrostMaxHoldoffShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_DefrostMaxHoldoffMet, &actualState);
      CHECK_EQUAL(expectedState, actualState);
   }

   void DefrostPowersBackUpIntoPrechillPrepAndCompressorStateIsMinimumOff()
   {
      Given FreezerEvaporatorThermistorValidityIs(VALID);
      Given DefrostStateIs(DefrostState_Prechill);
      Given CompressorStateIs(CompressorState_MinimumOffTime);
      Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

      After(PowerUpDelayInMs - 1);
      DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

      After(1);
      DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   }

   void DefrostIsInPrechillAndDefrostMaxHoldoffHasBeenSet()
   {
      Given FreezerEvaporatorThermistorValidityIs(VALID);
      Given MaxPrechillTimeInMinutesIs(defrostData.defrostMaxHoldoffTimeInMinutes);
      Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

      When CompressorStateTimeIsSatisfied();
      DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

      After((defrostData.defrostMaxHoldoffTimeInMinutes) * MSEC_PER_MIN - 1);
      DefrostMaxHoldoffShouldBe(CLEAR);

      After(1);
      DefrostMaxHoldoffShouldBe(SET);
   }

   void MaxPrechillTimeInMinutesIs(uint8_t minutes)
   {
      DataModel_Write(dataModel, Erd_MaxPrechillTimeInMinutes, &minutes);
   }

   void PrechillTimeMetErdShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_PrechillTimeMet, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void ValveHasBeenInPositionForThisManyMinutes(uint16_t minutes)
   {
      DataModel_Write(dataModel, Erd_TimeInMinutesInValvePositionB, &minutes);
   }

   void FreezerEvaporatorFilteredTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolved, &temperature);
   }

   void FreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolved, &temperature);
   }

   void FreezerAdjustedSetpointIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_AdjustedSetpoint, &temperature);
   }

   void PrechillTimeMetErdIs(bool state)
   {
      DataModel_Write(dataModel, Erd_PrechillTimeMet, &state);
   }

   void NoFreezeLimitIsActive()
   {
      DataModel_Write(dataModel, Erd_NoFreezeLimitIsActive, set);
   }

   void FreshFoodDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &actualVote);

      CHECK_FALSE(actualVote.care);
   }

   void FreezerDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actualVote);

      CHECK_FALSE(actualVote.care);
   }

   void IceCabinetFanVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actualVote;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_DefrostVote, &actualVote);

      CHECK_FALSE(actualVote.care);
   }

   void FreezerEvapFanShouldBeVotedWith(FanSpeed_t speed)
   {
      FanVotedSpeed_t actualVote;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actualVote);

      CHECK_EQUAL(actualVote.speed, speed);
      CHECK_TRUE(actualVote.care);
   }

   void FreshFoodEvapFanShouldBeVotedWith(FanSpeed_t speed)
   {
      FanVotedSpeed_t actualVote;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_DefrostVote, &actualVote);

      CHECK_EQUAL(actualVote.speed, speed);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerEvapFanVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actualVote;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actualVote);

      CHECK_FALSE(actualVote.care);
   }

   void FreshFoodEvapFanVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actualVote;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_DefrostVote, &actualVote);

      CHECK_FALSE(actualVote.care);
   }
};

TEST(Defrost_SingleEvap, ShouldInitializeIntoPowerUpHsmState)
{
   Given CalculatedGridLinesAre(calcGridLines);
   Given DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAtPowerUpWhenLastDefrostStateWasPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToDwellWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateWasHeaterOnAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_HeaterOn);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsPrechillAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Prechill);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsIdleAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Idle);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsDwellAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Dwell);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsDisabledAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldIncrementFreezerAbnormalDefrostCountWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasNormalAndLastFreezerDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasNormal();
   Given LastAbnormalFreezerDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   FreezerAbnormalDefrostCountShouldBe(0);

   After(1);
   FreezerAbnormalDefrostCountShouldBe(1);
}

TEST(Defrost_SingleEvap, ShouldSetLastFreezerDefrostAsAbnormalWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasNormalAndLastFreezerDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasNormal();
   Given LastAbnormalFreezerDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   LastFreezerDefrostWasNormal();

   After(1);
   LastFreezerDefrostShouldBeAbnormal();
}

TEST(Defrost_SingleEvap, ShouldSaveLastFreezerAbnormalDefrostCountWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasNormalAndLastFreezerDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasNormal();
   Given LastAbnormalFreezerDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   LastAbnormalFreezerDefrostCycleShouldBe(32);

   After(1);
   LastAbnormalFreezerDefrostCycleShouldBe(35);
}

TEST(Defrost_SingleEvap, ShouldSetLastFreezerDefrostAsAbnormalWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasNormalAndLastFreezerDefrostCycleCountThatWasAbnormalEqualsCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasNormal();
   Given LastAbnormalFreezerDefrostCycleWas(35);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   LastFreezerDefrostWasNormal();

   After(1);
   LastFreezerDefrostShouldBeAbnormal();
}

TEST(Defrost_SingleEvap, ShouldSaveLastFreezerAbnormalDefrostCountWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasAbnormalAndLastFreezerDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasAbnormal();
   Given LastAbnormalFreezerDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs - 1);
   LastAbnormalFreezerDefrostCycleShouldBe(32);

   After(1);
   LastAbnormalFreezerDefrostCycleShouldBe(35);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenLastFreezerDefrostWasNormalAndDefrostStateWasPrechillAfterPowerUpDelay)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostStateIs(DefrostState_Prechill);
   Given CompressorStateIs(CompressorState_MinimumOffTime);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPostPrechillWhenLastFreezerDefrostWasAbnormalAndDefrostStateWasPrechillAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Prechill);
   Given CompressorStateIs(CompressorState_MinimumOnTime);
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnWhenLastFreezerDefrostWasNormalAndDefrostStateWasHeaterOnAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_HeaterOn);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenLastFreezerDefrostWasNormalAndDefrostStateWasDwellAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Dwell);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenLastFreezerDefrostWasNormalAndDefrostStateWasDisabledAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenFreezerEvapThermistorIsInvalidOnEntryToPrechillPrep)
{
   Given DefrostStateIs(DefrostState_Prechill);
   Given FreezerEvaporatorThermistorValidityIs(INVALID);
   Given CompressorStateIs(CompressorState_MinimumOnTime);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

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
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When FreezerEvaporatorThermistorValidityIs(INVALID);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
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

TEST(Defrost_SingleEvap, ShouldVoteForFreshFoodHeaterOffOnEntryToPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   FreshFoodHeaterShouldBeVoted(OFF);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreezerHeaterOffOnEntryToPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   FreezerHeaterShouldBeVoted(OFF);
}

TEST(Defrost_SingleEvap, ShouldVoteForIceCabinetFanHighOnEntryToPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

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
   Given FreezerResolvedSetpointIs(defrostData.prechillFreezerSetpointInDegFx100);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreezerSetPointShouldBeVotedWith(defrostData.prechillFreezerSetpointInDegFx100);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreezerSetPointWithPrechillFreezerSetpointOnEntryToPrechillPrepAndResolvedFreezerSetpointIsGreaterThanPrechillSetpoint)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given FreezerResolvedSetpointIs(TemperatureGreaterThanPrechillFreezerSetpoint);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreezerSetPointShouldBeVotedWith(defrostData.prechillFreezerSetpointInDegFx100);
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

   ValvePositionShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldVoteForFreshFoodSetpointIfCurrentDefrostIsNotFreshFoodOnlyOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given CurrentDefrostIsNotFreshFoodOnly();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreshFoodSetpointShouldBeVotedWith(defrostData.prechillFreshFoodSetpointInDegFx100);
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
   Given SealedSystemValveIsInPosition(ValvePosition_C);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   ExtendDefrostSignalShouldNotBeSent();
}

TEST(Defrost_SingleEvap, ShouldRequestToEnableDoorHoldoffOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   DoorHoldoffRequestShouldBe(ENABLED);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostPrechillFromPrechillPrepWhenLastDefrostChangesFromNormalToAbnormal)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   When LastFreezerDefrostWasAbnormal();
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostPrechillFromPrechillWhenLastDefrostChangesFromNormalToAbnormal)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   When LastFreezerDefrostWasAbnormal();
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

TEST(Defrost_SingleEvap, ShouldIncrementPrechillRunCounterEveryMinuteWhileInPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given MaxPrechillTimeInMinutesIs(defrostData.defrostMaxHoldoffTimeInMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(1 * MSEC_PER_MIN - 1);
      PrechillRunCounterInMinutesShouldBe(i);

      After(1);
      PrechillRunCounterInMinutesShouldBe(i + 1);
   }
}

TEST(Defrost_SingleEvap, ShouldSetDefrostMaxHoldoffErdAfterInPrechillStateForDefrostMaxHoldoffTime)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given MaxPrechillTimeInMinutesIs(defrostData.defrostMaxHoldoffTimeInMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After((defrostData.defrostMaxHoldoffTimeInMinutes * MSEC_PER_MIN) - 1);
   DefrostMaxHoldoffShouldBe(CLEAR);

   After(1);
   DefrostMaxHoldoffShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldClearDefrostMaxHoldoffErdOnEntryToPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostMaxHoldoffIs(SET);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostMaxHoldoffShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldSetDefrostMaxHoldoffErdAfterInPrechillStateForDefrostMaxHoldoffTimeAgain)
{
   Given DefrostIsInPrechillAndDefrostMaxHoldoffHasBeenSet();

   When DefrostPowersBackUpIntoPrechillPrepAndCompressorStateIsMinimumOff();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After((defrostData.defrostMaxHoldoffTimeInMinutes) * MSEC_PER_MIN - 1);
   DefrostMaxHoldoffShouldBe(CLEAR);

   After(1);
   DefrostMaxHoldoffShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostPrechillIfFreezerDefrostIsAbnormalAndCompressorStateTimeIsNotSatisfiedOnEntryToPrechillPrep)
{
   Given DefrostStateIs(DefrostState_Prechill);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given CompressorStateIs(CompressorState_MinimumOnTime);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillIfFreezerDefrostIsNormalAndCompressorStateTimeIsSatisfiedOnEntryToPrechillPrep)
{
   Given DefrostStateIs(DefrostState_Prechill);
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given CompressorStateIs(CompressorState_On);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldNotSetDefrostMaxHoldoffMetErdWhenInAStateOtherThanPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After((defrostData.defrostMaxHoldoffTimeInMinutes) * MSEC_PER_MIN - 1);
   DefrostMaxHoldoffShouldBe(CLEAR);

   When FreezerEvaporatorThermistorValidityIs(INVALID);
   After(1);
   DefrostMaxHoldoffShouldBe(CLEAR);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldRequestToDisableDoorHoldoffOnExitOfPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   DoorHoldoffRequestShouldBe(ENABLED);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After((defrostData.defrostMaxHoldoffTimeInMinutes) * MSEC_PER_MIN - 1);
   When FreezerEvaporatorThermistorValidityIs(INVALID);

   After(1);
   DoorHoldoffRequestShouldBe(DISABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldSetPrechillTimeMetErdWhenPrechillMinuteRunCounterReachesMaxPrechillTime)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given MaxPrechillTimeInMinutesIs(SomeMaxPrechillTimeInMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After((SomeMaxPrechillTimeInMinutes * MSEC_PER_MIN) - 1);
   PrechillTimeMetErdShouldBe(CLEAR);

   After(1);
   PrechillRunCounterInMinutesShouldBe(SomeMaxPrechillTimeInMinutes);
   PrechillTimeMetErdShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldSetPrechillTimeMetWhenThreeWayValveTimePriorToPrechillCountsAsPrechillTimeIsSetInParametricAndValveHasBeenInPositionForMaxPrechillTime)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given MaxPrechillTimeInMinutesIs(SomeMaxPrechillTimeInMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When ValveHasBeenInPositionForThisManyMinutes(SomeMaxPrechillTimeInMinutes - 1);
   PrechillTimeMetErdShouldBe(CLEAR);

   When ValveHasBeenInPositionForThisManyMinutes(SomeMaxPrechillTimeInMinutes);
   PrechillTimeMetErdShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldNotSetPrechillTimeMetIfThreeWayValveTimePriorToPrechillCountsAsPrechillTimeIsClearInParametricAndValveHasBeenInPositionForMaxPrechillTime)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given MaxPrechillTimeInMinutesIs(SomeMaxPrechillTimeInMinutes);
   Given DefrostParametricSetWithThreeWayValveTimePriorToPrechillCountsAsPrechillTimeCleared();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateTimeIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When ValveHasBeenInPositionForThisManyMinutes(SomeMaxPrechillTimeInMinutes - 1);
   PrechillTimeMetErdShouldBe(CLEAR);

   When ValveHasBeenInPositionForThisManyMinutes(SomeMaxPrechillTimeInMinutes);
   PrechillTimeMetErdShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostPrechillWhenFreezerEvaporatorTemperatureUpdatesToLessThanPrechillExitFreezerTemperatureDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   Given FreezerEvaporatorFilteredTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   And FreezerEvaporatorFilteredTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 - 1);

   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_SingleEvap, ShouldNotTransitionToPostPrechillWhenFreezerEvaporatorTemperatureUpdatesToPrechillExitFreezerTemperatureDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   Given FreezerEvaporatorFilteredTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When FreezerEvaporatorFilteredTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenNoFreezeLimitIsActiveDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When NoFreezeLimitIsActive();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldNotTransitionToPostPrechillWhenFreezerCabinetTemperatureUpdatesToLessThanAdjustedFreezerSetpointDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   Given FreezerAdjustedSetpointIs(SomeAdjustedFreezerSetpointDegFx100);
   Given FilteredFreezerCabinetTemperatureIs(SomeAdjustedFreezerSetpointDegFx100 + 1);

   When FilteredFreezerCabinetTemperatureIs(SomeAdjustedFreezerSetpointDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenCompressorTurnsOffDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   Given CompressorStateIs(CompressorState_On);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostPrechillWhenPrechillTimeMetDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   And PrechillTimeMetErdIs(SET);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenNoFreezeLimitIsActiveAndCompressorIsNotOffOnEntryToPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When NoFreezeLimitIsActive();
   When CompressorStateTimeIsSatisfied();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenCompressorIsOffAndNoFreezeLimitIsActiveOnEntryToPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When NoFreezeLimitIsActive();
   When CompressorStateIs(CompressorState_Off);

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostPrechillWhenCompressorIsOffAndNoFreezeLimitIsNotActiveOnEntryToPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldVoteForValvePositionIfMaxPrechillHoldoffTimeIsGreaterThanZeroOnEntryToPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   ValvePositionShouldBeVotedWith(defrostData.threeWayValvePositionForMaxPrechillHoldoff);
}

TEST(Defrost_SingleEvap, ShouldNotVoteForValvePositionIfMaxPrechillHoldoffTimeIsZeroOnEntryToPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostParametricSetWithMaxPrechillHoldoffOfZero();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   ValvePositionShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryIfCompressorStateIsNotMinimumOnTimeOnEntryToPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenCompressorStateChangesToNotMinimumOnTimeDuringPostPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   And PrechillTimeMetErdIs(SET);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);

   When CompressorStateIs(CompressorState_MinimumRunTime);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenCompressorStateChangesToNotMinimumOnTimeAndIsOffDuringPostPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   And PrechillTimeMetErdIs(SET);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreezerEvapFanLowAndFreshFoodEvapFanOffOnEntryToPostPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   And PrechillTimeMetErdIs(SET);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);

   FreezerEvapFanShouldBeVotedWith(FanSpeed_Low);
   FreshFoodEvapFanShouldBeVotedWith(FanSpeed_Off);
}

TEST(Defrost_SingleEvap, ShouldVoteDontCareForDefrostHeatersAndIceBoxFanOnExitOfPrechillPrepToHeaterOnEntry)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(INVALID);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   FreezerDefrostHeaterVoteShouldBeDontCare();
   IceCabinetFanVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldVoteDontCareForFreezerAndFreshFoodEvapFansOnExitOfPrechillPrepToHeaterOnEntry)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(INVALID);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   FreezerEvapFanVoteShouldBeDontCare();
   FreshFoodEvapFanVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldVoteDontCareForDefrostHeatersAndIceBoxFanOnExitOfPrechillToHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   FreezerDefrostHeaterVoteShouldBeDontCare();
   IceCabinetFanVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldVoteDontCareForFreezerAndFreshFoodEvapFansOnExitOfPrechillToHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   FreezerEvapFanVoteShouldBeDontCare();
   FreshFoodEvapFanVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldVoteDontCareForDefrostHeatersAndIceBoxFanOnExitOfPostPrechillToHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   FreezerDefrostHeaterVoteShouldBeDontCare();
   IceCabinetFanVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldVoteDontCareForFreezerAndFreshFoodEvapFansOnExitOfPostPrechillToHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   FreezerEvapFanVoteShouldBeDontCare();
   FreshFoodEvapFanVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldVoteDontCareForValvePositionOnExitOfPostPrechillToHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   ValvePositionShouldBeDontCare();
}

TEST_GROUP(Defrost_DualEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   Defrost_t instance;
   DefrostData_t defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      DefrostData_TestDouble_Init(&defrostData);
      DefrostData_TestDouble_SetMaxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds(&defrostData, 60);

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
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolved, &temperature);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
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

   void DefrostInitializedWithNormalFreezerCabinetTemperatures()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given LastFreezerDefrostWasNormal();
      Given DefrostIsInitialized();
   }

   void LastFreezerDefrostWasAbnormalButCurrentFreezerCabinetTemperatureIsNotAbnormal()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given LastFreezerDefrostWasAbnormal();
      Given DefrostIsInitialized();
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      switch(state)
      {
         case DefrostHsmState_Idle:
            Given DefrostStateIs(DefrostState_Idle);
            Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

            After(PowerUpDelayInMs - 1);
            DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostStateIs(DefrostState_Prechill);
            Given FreezerEvaporatorThermistorValidityIs(VALID);
            Given CompressorStateIs(CompressorState_MinimumOnTime);
            Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

            After(PowerUpDelayInMs - 1);
            DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         case DefrostHsmState_Prechill:
            Given FreezerEvaporatorThermistorValidityIs(VALID);
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

            When CompressorStateTimeIsSatisfied();
            DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
            break;
      }
   }

   void SealedSystemValveIsInPosition(ValvePosition_t position)
   {
      DataModel_Write(dataModel, Erd_SealedSystemValvePosition, &position);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorIsValid, &state);
   }

   void FreezerSetPointShouldBeVotedWith(TemperatureDegFx100_t expectedTemperature)
   {
      SetpointVotedTemperature_t actualVote;
      DataModel_Read(dataModel, Erd_FreezerSetpoint_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedTemperature, actualVote.temperature);
      CHECK_TRUE(actualVote.care);
   }

   void FreezerResolvedSetpointIs(TemperatureDegFx100_t temperature)
   {
      SetpointVotedTemperature_t vote;
      vote.temperature = temperature;
      vote.care = true;
      DataModel_Write(dataModel, Erd_FreezerSetpoint_ResolvedVote, &vote);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void CompressorStateTimeIsSatisfied()
   {
      CompressorState_t state = CompressorState_On;
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void FreezerAdjustedSetpointIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_AdjustedSetpoint, &temperature);
   }

   void DoorHoldoffTimeIsSatisfiedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostDoorHoldoffTimeSatisfied, &state);
   }

   void DefrostMaxHoldoffIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostMaxHoldoffMet, &state);
   }

   void PrechillTimeMetErdIs(bool state)
   {
      DataModel_Write(dataModel, Erd_PrechillTimeMet, &state);
   }

   void NoFreezeLimitIsActive()
   {
      DataModel_Write(dataModel, Erd_NoFreezeLimitIsActive, set);
   }

   void FreezerEvapFanVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actualVote;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actualVote);

      CHECK_FALSE(actualVote.care);
   }

   void FreshFoodEvapFanVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actualVote;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_DefrostVote, &actualVote);

      CHECK_FALSE(actualVote.care);
   }
};

TEST(Defrost_DualEvap, ShouldVoteForFreezerSetPointWithPrechillFreezerSetpointOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given CompressorStateIs(CompressorState_MinimumOffTime);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreezerSetPointShouldBeVotedWith(defrostData.prechillFreezerSetpointInDegFx100);
}

TEST(Defrost_DualEvap, ShouldTransitionToPostPrechillWhenFreezerCabinetTemperatureUpdatesToLessThanAdjustedFreezerSetpointDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   Given CompressorStateIs(CompressorState_MinimumOnTime);
   Given FreezerAdjustedSetpointIs(SomeAdjustedFreezerSetpointDegFx100);
   Given FilteredFreezerCabinetTemperatureIs(SomeAdjustedFreezerSetpointDegFx100 + 1);

   When FilteredFreezerCabinetTemperatureIs(SomeAdjustedFreezerSetpointDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_DualEvap, ShouldNotTransitionToPostPrechillWhenFreezerCabinetTemperatureUpdatesToAdjustedFreezerSetpointDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   Given FreezerAdjustedSetpointIs(SomeAdjustedFreezerSetpointDegFx100);
   Given FilteredFreezerCabinetTemperatureIs(SomeAdjustedFreezerSetpointDegFx100 + 1);

   When FilteredFreezerCabinetTemperatureIs(SomeAdjustedFreezerSetpointDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvap, ShouldNotTransitionToPostPrechillWhenCompressorTurnsOffDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   Given CompressorStateIs(CompressorState_On);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvap, ShouldNotTransitionToPostPrechillWhenJustPrechillTimeMetDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When PrechillTimeMetErdIs(SET);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvap, ShouldTransitionToPostPrechillWhenDoorHoldoffMetThenPrechillTimeMetAndDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   When DoorHoldoffTimeIsSatisfiedIs(true);
   When PrechillTimeMetErdIs(SET);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_DualEvap, ShouldTransitionToPostPrechillWhenMaxHoldoffMetThenPrechillTimeMetDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   When DefrostMaxHoldoffIs(SET);
   When PrechillTimeMetErdIs(SET);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_DualEvap, ShouldTransitionToPostPrechillWhenPrechillTimeMetThenDoorHoldoffMetDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   When PrechillTimeMetErdIs(SET);
   When DoorHoldoffTimeIsSatisfiedIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_DualEvap, ShouldTransitionToPostPrechillWhenPrechillTimeMetThenMaxHoldoffMetDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   When PrechillTimeMetErdIs(SET);
   When DefrostMaxHoldoffIs(SET);
   DefrostHsmStateShouldBe(DefrostHsmState_PostPrechill);
}

TEST(Defrost_DualEvap, ShouldNotTransitionToHeaterOnEntryWhenNoFreezeLimitIsActiveDuringPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When NoFreezeLimitIsActive();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvap, ShouldNotTransitionToHeaterOnEntryWhenNoFreezeLimitIsActiveOnEntryToPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When NoFreezeLimitIsActive();
   When CompressorStateTimeIsSatisfied();

   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvap, ShouldNotTransitionToPostPrechillWhenCompressorIsOffAndNoFreezeLimitIsActiveOnEntryToPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When NoFreezeLimitIsActive();
   When CompressorStateIs(CompressorState_Off);

   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvap, ShouldNotTransitionToPostPrechillWhenCompressorIsOffAndNoFreezeLimitIsNotActiveOnEntryToPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorStateIs(CompressorState_Off);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvap, ShouldVoteDontCareForFreezerEvapFanLowAndFreshFoodEvapFanOffOnEntryToPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   FreezerEvapFanVoteShouldBeDontCare();
   FreshFoodEvapFanVoteShouldBeDontCare();
}
