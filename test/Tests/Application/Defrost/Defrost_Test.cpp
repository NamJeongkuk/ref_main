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
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
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
   .defrostReadyTimerIsSatisfied = Erd_DefrostReadyTimerIsSatisfied,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
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

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .gridLinesDegFx100 = { 0, -450, 150, 450, 950, 1150 }
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .gridLinesDegFx100 = { -250, 0, 250, 600, 750, 5500 }
};

static CalculatedGridLines_t calcGridLines = {
   .freshFoodGridLine = freshFoodCalcAxis,
   .freezerGridLine = freezerCalcAxis
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

   void LastConvertibleCompartmentDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void LastConvertibleCompartmentDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
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
      Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1);
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

   void DefrostReadyTimerIsSatisfied()
   {
      DataModel_Write(dataModel, Erd_DefrostReadyTimerIsSatisfied, set);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }
};

TEST(Defrost_SingleEvap, ShouldInitializeIntoPowerUpHsmState)
{
   Given CalculatedGridLinesAre(calcGridLines);
   Given DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);
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

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndLastFreshFoodDefrostWasAbnormal)
{
   Given LastFreshFoodDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndLastFreezerDefrostWasAbnormal)
{
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndLastConvertibleCompartmentDefrostWasAbnormal)
{
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndFreezerWasTooWarmAtPowerUpAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndFreezerWasTooWarmAtPowerUpAndLastFreezerDefrostWasAbnormalAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And LastFreezerDefrostWasAbnormal();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenDefrostReadyTimerIsSatisfiedAndLastDefrostsWereNormal)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerEvaporatorThermistorValidityIs(VALID);
   And CompressorStateIs(CompressorState_MinimumOffTime);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
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

TEST(Defrost_SingleEvap, ShouldNotVoteForValvePositionIfMaxPrechillHoldoffTimeIsEqualToZeroOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostParametricSetWithMaxPrechillHoldoffOfZero();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   ValvePositionShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldNotVoteForFreshFoodSetpointIfCurrentDefrostIsFreshFoodOnlyOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given CurrentDefrostIsFreshFoodOnly();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   FreshFoodSetpointShouldNotBeVotedFor();
}

TEST(Defrost_SingleEvap, ShouldNotRequestToExtendDefrostWithFreshFoodCycleDefrostIfCurrentValvePositionIsNotInPositionToExtendDefrostOnEntryToPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given SealedSystemValveIsInPosition(ValvePosition_C);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   ExtendDefrostSignalShouldNotBeSent();
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

   void DefrostInitializedWithNormalFreezerCabinetTemperatures()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given LastFreezerDefrostWasNormal();
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
      }
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorIsValid, &state);
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

TEST(Defrost_DualEvap, ShouldVoteDontCareForFreezerEvapFanLowAndFreshFoodEvapFanOffOnEntryToPostPrechill)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostPrechill);

   FreezerEvapFanVoteShouldBeDontCare();
   FreshFoodEvapFanVoteShouldBeDontCare();
}
