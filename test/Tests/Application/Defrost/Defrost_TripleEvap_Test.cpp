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
#include "Vote.h"
#include "DefrostConfiguration.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   Invalid = false,
   Valid = true
};

enum
{
   ZeroMinutes = 0,
};

enum
{
   FreezerDefrostHeaterMaxOnTimeInMinutes = 45
};

enum
{
   CaresAbout = Vote_Care,
   DoesNotCareAbout = Vote_DontCare,
};

enum
{
   SixGridLines = 6,
   TwoDimensional = 2,
};

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .gridLinesDegFx100 = { 0, -450, 150, 450, 950, 1150 }
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .gridLinesDegFx100 = { -250, 0, 250, 600, 750, 6000 }
};

static TwoDimensionalCalculatedGridLines_t calcGridLines = {
   .firstDimensionGridLines = freshFoodCalcAxis,
   .secondDimensionGridLines = freezerCalcAxis,
   .numberOfGridLinesPerDimension = 6
};

static const DefrostIdleData_t idleData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .ahamPrechillTimeBetweenDefrostsInMinutes = 6 * MINUTES_PER_HOUR,
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 2,
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet = 1
};

static const DefrostPrechillPrepData_t prechillPrepData = {
   .maxPrechillPrepTimeInMinutes = 5
};

static const DefrostPrechillData_t prechillData = {
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForSecondaryOnlyDefrostInMinutes = 20,
   .prechillSealedSystemValvePosition = SealedSystemValvePosition_B,
   .prechillCompressorSpeed = CompressorSpeed_Low,
   .prechillFreezerEvaporatorFanSpeed = FanSpeed_SuperLow,
   .prechillFreshFoodEvaporatorFanSpeed = FanSpeed_Low,
   .prechillIceCabinetFanSpeed = FanSpeed_Medium,
   .prechillDeliFanSpeed = FanSpeed_High,
   .prechillConvertibleCompartmentDamperPosition = DamperPosition_Open,
   .prechillDeliDamperPosition = DamperPosition_Open,
   .prechillConvertibleCompartmentEvapFanSpeed = FanSpeed_High,
   .prechillFreshFoodDamperPosition = DamperPosition_Closed,
   .prechillFreezerMinTempInDegFx100 = -600,
   .prechillFreshFoodMinTempInDegFx100 = 3200,
   .prechillFreshFoodMaxTempInDegFx100 = 4600,
   .prechillConvertibleCompartmentAsFreezerMinTempInDegFx100 = -600,
   .prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 = 3200,
   .prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100 = 4600,
   .prechillPrimaryEvaporatorExitTemperatureInDegFx100 = -3000
};

static const DefrostHeaterOnEntryData_t heaterOnEntryData = {
   .defrostHeaterOnDelayAfterCompressorOffInSeconds = 2,
   .heaterOnEntryFreshFoodDamperPosition = DamperPosition_Open,
   .heaterOnEntryConvertibleCompartmentDamperPosition = DamperPosition_Closed,
   .heaterOnEntrySealedSystemValvePosition = SealedSystemValvePosition_A
};

static const DefrostHeaterData_t freshFoodHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 60,
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .defrostTerminationTemperatureInDegFx100 = 4460
};

static const DefrostHeaterData_t freezerHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 60,
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .defrostTerminationTemperatureInDegFx100 = 5900
};

static DefrostConvertibleCompartmentCommonHeaterData_t convertibleCompartmentHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 45,
   .defrostTerminationTemperatureInDegFx100 = 4460
};

static const DefrostConvertibleCompartmentStateDependentHeaterData_t convertibleCompartmentHeaterAsFreshFood = {
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 2,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 21,
};

static const DefrostConvertibleCompartmentStateDependentHeaterData_t convertibleCompartmentHeaterAsFreezer = {
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 25,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 42,
};

static const DefrostHeaterOnData_t heaterOnData = {
   .freshFoodHeater = freshFoodHeater,
   .freezerHeater = freezerHeater,
   .convertibleCompartmentHeater = convertibleCompartmentHeater,
   .convertibleCompartmentHeaterAsFreshFood = convertibleCompartmentHeaterAsFreshFood,
   .convertibleCompartmentHeaterAsFreezer = convertibleCompartmentHeaterAsFreezer
};
static const DefrostDwellData_t dwellData = {
   .dwellTimeInMinutes = 7,
   .dwellFreshFoodDamperPosition = DamperPosition_Closed,
   .dwellSealedSystemValvePosition = SealedSystemValvePosition_B
};

static const DefrostPostDwellData_t postDwellData = {
   .postDwellExitTimeInMinutes = 5,
   .secondaryOnlyPostDwellExitTimeInMinutes = 10,
   .postDwellPrimaryEvaporatorExitTemperatureInDegFx100 = -1000,
   .secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100 = -1200,
   .postDwellSealedSystemValvePosition = SealedSystemValvePosition_B,
   .postDwellCompressorSpeed = CompressorSpeed_Low,
   .postDwellCondenserFanSpeed = FanSpeed_Low,
   .postDwellFreshFoodDamperPosition = DamperPosition_Closed,
   .postDwellConvertibleCompartmentDamperPosition = DamperPosition_Closed,
   .postDwellDeliDamperPosition = DamperPosition_Closed
};

static const DefrostData_t defrostData = {
   .idleData = idleData,
   .prechillPrepData = prechillPrepData,
   .prechillData = prechillData,
   .heaterOnEntryData = heaterOnEntryData,
   .heaterOnData = heaterOnData,
   .dwellData = dwellData,
   .postDwellData = postDwellData
};

TEST_GROUP(Defrost_TripleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;
   EnhancedSabbathData_t enhancedSabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentTripleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_FreshFood);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Convertible);
      platformData.numberOfEvaporators = 3;

      enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 3;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenDefrostIsInitializedWithEepromClearedOnStartup()
   {
      GivenClearedEepromOnStartupIs(SET);
      Defrost_Init(&instance, dataModel, &defrostConfiguration, &defrostData, &platformData, &enhancedSabbathData);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DefrostStateIs(state);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfiguration, &defrostData, &platformData, &enhancedSabbathData);
   }

   void WhenPrechillConditionsAreMet()
   {
      ConvertibleCompartmentStateType_t stateType;
      DataModel_Read(
         dataModel,
         Erd_ConvertibleCompartmentState,
         &stateType);

      if(stateType == ConvertibleCompartmentStateType_Freezer)
      {
         CoolingModeIs(CoolingMode_ConvertibleCompartment);
      }
      else
      {
         CoolingModeIs(CoolingMode_Freezer);
      }
      CompressorIs(ON);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      switch(state)
      {
         case DefrostHsmState_Idle:
            GivenDefrostStateIs(DefrostState_Idle);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         case DefrostHsmState_Prechill:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

            WhenPrechillConditionsAreMet();
            DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
            break;

         case DefrostHsmState_HeaterOnEntry:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

            DefrostType_t currentDefrostType;
            DataModel_Read(dataModel, Erd_CurrentDefrostType, &currentDefrostType);

            if(currentDefrostType == DefrostType_SecondaryOnly)
            {
               After(defrostData.prechillData.maxPrechillTimeForSecondaryOnlyDefrostInMinutes * MSEC_PER_MIN);
            }
            else
            {
               After(defrostData.prechillData.maxPrechillTimeInMinutes * MSEC_PER_MIN);
            }
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
            break;

         case DefrostHsmState_HeaterOn:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

            After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
            break;

         default:
            break;
      }
   }

   void GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t convertibleCompartmentStateType)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &convertibleCompartmentStateType);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void GivenFilteredConvertibleCompartmentCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenFilteredConvertibleCompartmentEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(temperature);
   }

   void GivenConvertibleCompartmentEvapThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved, &state);
   }

   void CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_CabinetFilteredTemperatureTooWarmOnPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void GivenFilteredFreshFoodEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenFilteredFreshFoodEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenFilteredFreshFoodEvapTemperatureIs(temperature);
   }

   void GivenFreshFoodEvapThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvapThermistor_IsValidResolved, &state);
   }

   void GivenFreezerThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, set);
   }

   void GivenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }

   void GivenConvertibleCompartmentEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvapThermistorDiscovered, set);
   }

   void GivenConvertibleCompartmentEvapThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvapThermistorDiscovered, clear);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void ClearedEepromOnStartupIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &state);
   }

   void GivenClearedEepromOnStartupIs(bool state)
   {
      ClearedEepromOnStartupIs(state);
   }

   void NextDefrostTypeOverrideIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_NextDefrostTypeOverride, &defrostType);
   }

   void NextDefrostTypeOverrideShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_NextDefrostTypeOverride, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenCabinetIsTooWarmOnPowerUp()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
      Given CalculatedGridLinesAre(calcGridLines);

      CHECK_TRUE((defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100) < (freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh]));
   }

   void CalculatedGridLinesAre(TwoDimensionalCalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &gridLines);
   }

   void CurrentDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
   }

   void GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(uint8_t numberOfSecondaryOnlyDefrostsBeforeAFullDefrost)
   {
      DataModel_Write(dataModel, Erd_NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost, &numberOfSecondaryOnlyDefrostsBeforeAFullDefrost);
   }

   void NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenNumberOfSecondaryOnlyDefrostsIs(uint8_t numberOfSecondaryOnlyDefrosts)
   {
      DataModel_Write(dataModel, Erd_NumberOfSecondaryOnlyDefrosts, &numberOfSecondaryOnlyDefrosts);
   }

   void NumberOfSecondaryOnlyDefrostsShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_NumberOfSecondaryOnlyDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DisableDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DisableDefrost, &state);
   }

   void DefrostTestIsRequested(DefrostTestStateRequest_t request)
   {
      DefrostTestStateRequestMessage_t requestMessage;
      DataModel_Read(dataModel, Erd_DefrostTestStateRequest, &requestMessage);
      requestMessage.request = request;
      requestMessage.requestId++;
      DataModel_Write(dataModel, Erd_DefrostTestStateRequest, &requestMessage);
   }

   void WhenIdleIsExitedVia(DefrostTestStateRequest_t request)
   {
      DefrostTestIsRequested(request);
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathModeEnable, &state);
   }

   void WhenSabbathModeIs(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeEnable, &state);
   }

   void WhenEnhancedSabbathModeIs(bool state)
   {
      GivenEnhancedSabbathModeIs(state);
   }

   void GivenLastConvertibleCompartmentDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void WhenLastConvertibleCompartmentDefrostWasNormal()
   {
      GivenLastConvertibleCompartmentDefrostWasNormal();
   }

   void GivenLastConvertibleCompartmentDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void WhenLastConvertibleCompartmentDefrostWasAbnormal()
   {
      GivenLastConvertibleCompartmentDefrostWasAbnormal();
   }

   void GivenLastFreezerDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void WhenLastFreezerDefrostWasNormal()
   {
      GivenLastFreezerDefrostWasNormal();
   }

   void GivenLastFreezerDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void WhenLastFreezerDefrostWasAbnormal()
   {
      GivenLastFreezerDefrostWasAbnormal();
   }

   void GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      GivenConvertibleCompartmentStateIs(state);
   }

   void WhenSabbathIsReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_SabbathIsReadyToDefrost, set);
   }

   void WhenEnhancedSabbathIsRequestingToDefrost()
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathIsRequestingDefrost, set);
   }

   void FreezerDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreezerDefrostHeaterVoteShouldBe(HeaterState_t expected, Vote_t care)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_EQUAL(care, actual.care);
   }

   void FreshFoodDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodDefrostHeaterVoteShouldBe(HeaterState_t expected, Vote_t care)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_EQUAL(care, actual.care);
   }

   void ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_t expected, Vote_t care)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_EQUAL(care, actual.care);
   }

   void GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes, &timeInMinutes);
   }

   void WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes, &timeInMinutes);
   }

   void WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void InvalidEvaporatorThermistorDuringDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_InvalidEvaporatorThermistorDuringDefrost, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeaterOnForMaxTimeFault, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void InvalidEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidEvaporatorThermistorDuringDefrost, &state);
   }

   void NumberOfFreshFoodDefrostsIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreshFoodDefrosts, &count);
   }

   void NumberOfFreshFoodDefrostsShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfFreshFoodDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NumberOfFreshFoodAbnormalDefrostsIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreshFoodAbnormalDefrosts, &count);
   }

   void NumberOfFreshFoodAbnormalDefrostsShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfFreshFoodAbnormalDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void LastFreshFoodDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void FreshFoodDefrostWasAbnormalFlagShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostWasAbnormal, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NumberOfConvertibleCompartmentDefrostsIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfConvertibleCompartmentDefrosts, &count);
   }

   void NumberOfConvertibleCompartmentDefrostsShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfConvertibleCompartmentDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NumberOfConvertibleCompartmentAbnormalDefrostsIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfConvertibleCompartmentAbnormalDefrosts, &count);
   }

   void NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfConvertibleCompartmentAbnormalDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void LastConvertibleCompartmentDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void LastFreshFoodDefrostWasAbnormal()
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, set);
   }

   void LastConvertibleCompartmentDefrostWasAbnormal()
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, set);
   }

   void GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void GivenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void GivenFreshFoodDefrostHeaterOnForMaxTimeFaultIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeaterOnForMaxTimeFault, &state);
   }

   void GivenConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostHeaterOnForMaxTimeFault, &state);
   }

   void FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeaterOnForMaxTimeFault, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FreezerDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void FreezerDefrostHeaterMaxOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterMaxOnTimeInMinutes, &timeInMinutes);
   }

   void NumberOfFreezerDefrostsIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreezerDefrosts, &count);
   }

   void NumberOfFreezerDefrostsShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfFreezerDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_t type)
   {
      Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
      if(type == DefrostType_Full)
      {
         GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
      }
      Given CurrentDefrostTypeIs(type);
      GivenAllThermistorsAreValidAndHaveBeenDiscovered();
      GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
      Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
      CurrentDefrostTypeShouldBe(type);
   }

   void GivenAllThermistorsValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_FreshFoodEvapThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved, &state);
   }

   void GivenAllEvapThermistorsAreAtTemp(TemperatureDegFx100_t temp)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temp);
      DataModel_Write(dataModel, Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100, &temp);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvap_FilteredTemperatureResolvedInDegFx100, &temp);
   }

   void GivenCabinetIsNotTooWarmOnPowerUp()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 - 1);
      Given CalculatedGridLinesAre(calcGridLines);

      CHECK_TRUE((freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] - 1) > (defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 - 1));
   }

   void GivenDefrostStartsInDwellAndDwellTimerExpires()
   {
      GivenCabinetIsNotTooWarmOnPowerUp();
      And DefrostStateIs(DefrostState_Dwell);
      And DefrostIsInitialized();
      DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
      After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredConvertibleCompartmentEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenFilteredFreezerEvapTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      FilteredFreezerEvapTemperatureIs(temperature);
   }

   void WhenFilteredFreshFoodEvapTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      FilteredFreshFoodEvapTemperatureIs(temperature);
   }

   void WhenFilteredConvertibleCompartmentEvapTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      FilteredConvertibleCompartmentEvapTemperatureIs(temperature);
   }

   void GivenFreezerEvapThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, clear);
   }

   void GivenFreshFoodEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvaporatorThermistorDiscovered, set);
   }

   void GivenFreshFoodEvapThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvaporatorThermistorDiscovered, clear);
   }

   void WhenFreshFoodEvapThermistorValidityIs(bool state)
   {
      GivenFreshFoodEvapThermistorValidityIs(state);
   }

   void ConvertibleCompartmentThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved, &state);
   }

   void GivenConvertibleCompartmentCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenPrechillConditionsAreMetForTripleEvapAsConvertibleCompartment()
   {
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_ConvertibleCompartment);
   }

   void WhenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(temperature);
   }

   void ConvertibleCompartmentEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved, &state);
   }

   void WhenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }

   void WhenFreshFoodEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvaporatorThermistorDiscovered, set);
   }

   void WhenConvertibleCompartmentEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvaporatorThermistorDiscovered, set);
   }

   void GivenFreshFoodThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistorDiscovered, set);
   }

   void GivenConvertibleCompartmentThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinetThermistorDiscovered, set);
   }

   void GivenConvertibleCompartmentThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved, &state);
   }

   void GivenAllThermistorsAreValidAndHaveBeenDiscovered()
   {
      Given FreezerEvaporatorThermistorValidityIs(Valid);
      GivenFreezerEvapThermistorHasBeenDiscovered();

      Given FreezerThermistorValidityIs(Valid);
      GivenFreezerThermistorHasBeenDiscovered();

      GivenFreshFoodEvapThermistorValidityIs(Valid);
      GivenFreshFoodEvapThermistorHasBeenDiscovered();

      Given FreshFoodThermistorValidityIs(Valid);
      GivenFreshFoodThermistorHasBeenDiscovered();

      GivenConvertibleCompartmentThermistorValidityIs(Valid);
      GivenConvertibleCompartmentThermistorHasBeenDiscovered();

      GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
      GivenConvertibleCompartmentEvapThermistorHasBeenDiscovered();
   }

   void GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreezerEvapThermistorHasNotBeenDiscovered()
   {
      Given FreezerEvaporatorThermistorValidityIs(Valid);
      GivenFreezerEvapThermistorHasNotBeenDiscovered();
      Given FreezerThermistorValidityIs(Valid);
      GivenFreezerThermistorHasBeenDiscovered();
      GivenFreshFoodEvapThermistorValidityIs(Valid);
      GivenFreshFoodEvapThermistorHasBeenDiscovered();
      Given FreshFoodThermistorValidityIs(Valid);
      GivenFreshFoodThermistorHasBeenDiscovered();
      GivenConvertibleCompartmentThermistorValidityIs(Valid);
      GivenConvertibleCompartmentThermistorHasBeenDiscovered();
      GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
      GivenConvertibleCompartmentEvapThermistorHasBeenDiscovered();
   }

   void GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreshFoodEvapThermistorHasNotBeenDiscovered()
   {
      Given FreezerEvaporatorThermistorValidityIs(Valid);
      GivenFreezerEvapThermistorHasBeenDiscovered();
      Given FreezerThermistorValidityIs(Valid);
      GivenFreezerThermistorHasBeenDiscovered();
      GivenFreshFoodEvapThermistorValidityIs(Valid);
      GivenFreshFoodEvapThermistorHasNotBeenDiscovered();
      Given FreshFoodThermistorValidityIs(Valid);
      GivenFreshFoodThermistorHasBeenDiscovered();
      GivenConvertibleCompartmentThermistorValidityIs(Valid);
      GivenConvertibleCompartmentThermistorHasBeenDiscovered();
      GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
      GivenConvertibleCompartmentEvapThermistorHasBeenDiscovered();
   }

   void GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreshFoodEvapThermistorAndConvertibleCompartmentEvapThermistorHasNotBeenDiscovered()
   {
      Given FreezerEvaporatorThermistorValidityIs(Valid);
      GivenFreezerEvapThermistorHasBeenDiscovered();
      Given FreezerThermistorValidityIs(Valid);
      GivenFreezerThermistorHasBeenDiscovered();
      GivenFreshFoodEvapThermistorValidityIs(Valid);
      GivenFreshFoodEvapThermistorHasNotBeenDiscovered();
      Given FreshFoodThermistorValidityIs(Valid);
      GivenFreshFoodThermistorHasBeenDiscovered();
      GivenConvertibleCompartmentThermistorValidityIs(Valid);
      GivenConvertibleCompartmentThermistorHasBeenDiscovered();
      GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
      GivenConvertibleCompartmentEvapThermistorHasNotBeenDiscovered();
   }

   void GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet()
   {
      Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
      Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
      Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
      GivenFilteredConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   }
};

TEST(Defrost_TripleEvap, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCoolingModeIsFreezerAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenCompressorTurnsOnAndPrechillConditionsMetWhileInPrechillPrepAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenCoolingModeChangesToConvertibleCompartmentAndPrechillConditionsMetWhileInPrechillPrepAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When CoolingModeIs(CoolingMode_ConvertibleCompartment);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCoolingModeIsConvertibleCompartmentAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenCompressorTurnsOnAndPrechillConditionsMetWhileInPrechillPrepAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();

   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_Freezer);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenCoolingModeChangesToFreezerAndPrechillConditionsMetWhileInPrechillPrepAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When CoolingModeIs(CoolingMode_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToTrueWhenConvertibleCompartmentFilteredTemperatureIsEqualToDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToTrueWhenConvertibleCompartmentFilteredTemperatureIsAboveDefrostTerminationTemperatureAndThermistorIsValidAndIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100 + 1);
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_TripleEvap, ShouldNotSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToTrueWhenConvertibleCompartmentEvapFilteredTemperatureIsAboveDefrostTerminationTemperatureAndThermistorIsInvalidAndIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100 + 1);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Invalid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToFalseWhenConvertibleCompartmentEvapFilteredTemperatureIsBelowDefrostTerminationTemperatureAndThermistorIsValidIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100 - 1);
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToTrueWhenFreezerEvapFilteredTemperatureIsAtDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToTrueWhenFreezerEvapFilteredTemperatureIsAboveDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToFalseWhenFreezerEvapFilteredTemperatureIsBelowDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 - 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToFalseWhenFreshFoodEvapFilteredTemperatureIsAtDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToTrueWhenFreezerEvapFilteredTemperatureIsAtDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToTrueWhenFreezerEvapFilteredTemperatureIsAboveDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToFalseWhenConvertibleCompartmentEvapFilteredTemperatureIsBelowDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100 - 1);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToFalseWhenConvertibleCompartmentEvapFilteredTemperatureIsAtDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_TripleEvap, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToFalseWhenFreshFoodEvapFilteredTemperatureIsAtDefrostTerminationTemperatureAndThermistorIsValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_TripleEvap, ShouldSetNextDefrostTypeOverrideToFullIfEepromClearedOnPowerUp)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   GivenDefrostIsInitializedWithEepromClearedOnStartup();

   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(Defrost_TripleEvap, ShouldSetNextDefrostTypeOverrideToFullIfCabinetTooWarmOnPowerUp)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitialized();

   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(Defrost_TripleEvap, ShouldSetNextDefrostTypeOverrideToFullIfDefrostWasDisabledBeforePowerUp)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitialized();

   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(Defrost_TripleEvap, ShouldSetCurrentDefrostTypeToSecondaryOnlyDefrostOnIdleExitWhenNextDefrostTypeOverrideIsDontCareAndNumberOfSecondaryDefrostsIsLessThanNumberOfSecondaryOnlyDefrostsBeforeFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost - 1);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
}

TEST(Defrost_TripleEvap, ShouldSetCurrentDefrostTypeToFullOnIdleExitWhenNextDefrostTypeOverrideIsDontCareAndNumberOfSecondaryDefrostIsGreaterThanNumberOfSecondaryOnlyDefrostsBeforeFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost + 1);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_TripleEvap, ShouldSetCurrentDefrostTypeToFullOnIdleExitWhenNextDefrostTypeOverrideIsDontCareAndNumberOfSecondaryDefrostIsEqualToNumberOfSecondaryOnlyDefrostsBeforeFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_TripleEvap, ShouldSetCurrentDefrostTypeToNextDefrostTypeOverrideOnIdleExitWhenNextDefrostTypeIsSecondaryOnly)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
}

TEST(Defrost_TripleEvap, ShouldSetNextDefrostTypeOverrideToDontCareOnIdleExitWhenNextDefrostTypeIsSecondaryOnly)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NextDefrostTypeOverrideShouldBe(DefrostType_DontCare);
}

TEST(Defrost_TripleEvap, ShouldSetCurrentDefrostTypeToNextDefrostTypeOverrideAndIncrementNumberOfSecondaryDefrostsOnIdleExitWhenNextDefrostTypeIsSecondaryOnlyOnIdleExitViaPrechillTestRequest)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenNumberOfSecondaryOnlyDefrostsIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenIdleIsExitedVia(DefrostTestStateRequest_Prechill);
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
   NumberOfSecondaryOnlyDefrostsShouldBe(1);
}

TEST(Defrost_TripleEvap, ShouldSetCurrentDefrostTypeToNextDefrostTypeOverrideAndIncrementNumberOfSecondaryDefrostsOnIdleExitWhenNextDefrostTypeIsSecondaryOnlyOnIdleExitViaDefrostTestRequest)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenNumberOfSecondaryOnlyDefrostsIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenIdleIsExitedVia(DefrostTestStateRequest_Defrost);
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
   NumberOfSecondaryOnlyDefrostsShouldBe(1);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfSecondaryOnlyDefrostsOnIdleExitWhenNextDefrostTypeIsSecondaryOnlyOnIdleExit)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   GivenNumberOfSecondaryOnlyDefrostsIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsShouldBe(1);
}

TEST(Defrost_TripleEvap, ShouldSetNumberOfSecondaryOnlyDefrostsToZeroOnIdleExitWhenNextDefrostTypeIsNotSecondaryOnlyOnIdleExit)
{
   Given NextDefrostTypeOverrideIs(DefrostType_Full);
   GivenNumberOfSecondaryOnlyDefrostsIs(10);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsShouldBe(0);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfSecondaryOnlyDefrostOnIdleExitWhenNextDefrostTypeIsDontCareAndNumberOfSecondaryDefrostIsLessThanNumberOfSecondaryOnlyDefrostsBeforeFullDefrostOnIdleExit)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsShouldBe(1);
}

TEST(Defrost_TripleEvap, ShouldSetNumberOfSecondaryOnlyDefrostToZeroOnIdleExitWhenNextDefrostTypeIsDontCareAndNumberOfSecondaryDefrostIsGreaterThanNumberOfSecondaryOnlyDefrostsBeforeFullDefrostOnIdleExit)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsShouldBe(0);
}

TEST(Defrost_TripleEvap, ShouldSkipDefrostTypeDeterminationOnIdleExitWhenDefrostIsDisabled)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DisableDefrostIs(ENABLED);
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_TripleEvap, ShouldNotSkipDefrostTypeDeterminationOnIdleExitWhenDefrostIsDisabledAndThenEnabled)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DisableDefrostIs(ENABLED);
   CurrentDefrostTypeShouldBe(DefrostType_Full);

   When DisableDefrostIs(DISABLED);
   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
}

TEST(Defrost_TripleEvap, ShouldSetNumberOfSecondaryOnlyDefrostsOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldSetNumberOfSecondaryOnlyDefrostsToEnhancedSabbathDataWhenEnhancedSabbathIsEnabledOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenEnhancedSabbathModeIs(ENABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldSetNumberOfSecondaryOnlyDefrostsToEnhancedSabbathDataWhenSabbathIsEnabledOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenSabbathModeIs(ENABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldSetNumberOfSecondaryOnlyDefrostsToIdleAbnormalDataWhenFreezerDefrostWasAbnormalOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   GivenLastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(Defrost_TripleEvap, ShouldSetNumberOfSecondaryOnlyDefrostsToIdleAbnormalDataWhenConvertibleCompartmentDefrostWasAbnormalOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   GivenLastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(Defrost_TripleEvap, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenEnhancedSabbathModeIsEnabledOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenEnhancedSabbathModeIs(DISABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenEnhancedSabbathModeIs(ENABLED);
   And WhenSabbathIsReadyToDefrost();
   And WhenEnhancedSabbathIsRequestingToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenEnhancedSabbathModeIsDisabledOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenEnhancedSabbathModeIs(ENABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenEnhancedSabbathModeIs(DISABLED);
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenSabbathModeIsEnabledOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenSabbathModeIs(DISABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenSabbathModeIs(ENABLED);
   And WhenSabbathIsReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenSabbathModeIsDisabledOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenEnhancedSabbathModeIs(ENABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenEnhancedSabbathModeIs(DISABLED);
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenFreezerDefrostIsAbnormalOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   GivenLastFreezerDefrostWasNormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenLastFreezerDefrostWasAbnormal();
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(Defrost_TripleEvap, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenFreezerDefrostIsNormalOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   GivenLastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenLastFreezerDefrostWasNormal();
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenConvertibleCompartmentDefrostIsAbnormalOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   GivenLastConvertibleCompartmentDefrostWasNormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenLastConvertibleCompartmentDefrostWasAbnormal();
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(Defrost_TripleEvap, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenConvertibleCompartmentDefrostIsNormalOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   GivenLastConvertibleCompartmentDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenLastConvertibleCompartmentDefrostWasNormal();
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_TripleEvap, ShouldExitOnHeaterEntryStateAndTurnOnTheFreshFoodAndFreezerAndConvertibleCompartmentDefrostHeatersAfterDefrostHeaterOnDelayTimerExpiredWhenDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
}

TEST(Defrost_TripleEvap, ShouldExitOnHeaterEntryStateAndTurnOnTheFreshFoodDefrostHeaterAfterDefrostHeaterOnDelayTimerExpiredWhenDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
}

TEST(Defrost_TripleEvap, ShouldExitOnHeaterEntryStateAndTurnOnTheFreshFoodAndFreezerAndConvertibleCompartmentDefrostHeatersAfterDefrostHeaterOnDelayTimerExpiredWhenDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
}

TEST(Defrost_TripleEvap, ShouldExitOnHeaterEntryStateAndTurnOnTheFreshFoodDefrostHeaterAfterDefrostHeaterOnDelayTimerExpiredWhenDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();
}

TEST(Defrost_TripleEvap, ShouldSetInvalidEvaporatorThermistorDuringDefrostAndNotSetConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenTheConvertibleCompartmentEvapThermistorIsInvalidAndHasBeenDiscoveredAndMaxOnTimeReachesOnHeaterOnAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentEvapThermistorHasBeenDiscovered();
   GivenConvertibleCompartmentEvapThermistorValidityIs(Invalid);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(SET);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_TripleEvap, ShouldSetInvalidEvaporatorThermistorDuringDefrostAndNotSetConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenTheConvertibleCompartmentEvapThermistorIsInvalidAndHasBeenDiscoveredAndMaxOnTimeReachesOnHeaterOnAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentEvapThermistorHasBeenDiscovered();
   GivenConvertibleCompartmentEvapThermistorValidityIs(Invalid);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(SET);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_TripleEvap, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndSetConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenFreshFoodEvapAndFreezerEvapAndConvertibleCompartmentEvapThermistorsAreValidAndMaxOnTimeReachesOnHeaterOnAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndSetConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenFreshFoodEvapAndFreezerEvapAndConvertibleCompartmentEvapThermistorsAreValidAndMaxOnTimeReachesOnHeaterOnAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldTransitionFromHeaterOnToDwellAfterFreshFoodHeaterIsOnForMaxTimeWhenDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodEvapThermistorValidityIs(Valid);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldTransitionFromHeaterOnToDwellAfterConvertibleCompartmentHeaterIsOnForMaxTimeWhenDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldTransitionFromHeaterOnToDwellAfterFreshFoodHeaterIsOnForMaxTimeWhenDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfFreshFoodAbnormalDefrostCycleCountAndSetFreshFoodDefrostWasAbnormalWhenFreshFoodHeaterOnTimeReachesAbnormalTimeAndDefrostTypeIsSecondaryOnly)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NumberOfFreshFoodAbnormalDefrostsIs(1);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   Given LastFreshFoodDefrostWasNormal();
   Given NumberOfFreshFoodDefrostsIs(3);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreshFoodAbnormalDefrostsShouldBe(2);
   And FreshFoodDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfFreshFoodAbnormalDefrostCycleCountJustOnceAndDefrostTypeIsSecondaryOnly)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NumberOfFreshFoodAbnormalDefrostsIs(1);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreshFoodAbnormalDefrostsShouldBe(2);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes + 1);
   NumberOfFreshFoodAbnormalDefrostsShouldBe(2);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfConvertibleCompartmentAbnormalDefrostCycleCountAndSetConvertibleCompartmentDefrostWasAbnormalWhenConvertibleCompartmentHeaterOnTimeReachesAbnormalTimeAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given NumberOfConvertibleCompartmentAbnormalDefrostsIs(1);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   Given LastConvertibleCompartmentDefrostWasNormal();
   Given NumberOfConvertibleCompartmentDefrostsIs(3);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);
   And ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfConvertibleCompartmentAbnormalDefrostCycleCountJustOnceAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given NumberOfConvertibleCompartmentAbnormalDefrostsIs(1);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes + 1);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfConvertibleCompartmentAbnormalDefrostCycleCountAndSetConvertibleCompartmentDefrostWasAbnormalWhenConvertibleCompartmentHeaterOnTimeReachesAbnormalTimeAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NumberOfConvertibleCompartmentAbnormalDefrostsIs(1);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   Given LastConvertibleCompartmentDefrostWasNormal();
   Given NumberOfConvertibleCompartmentDefrostsIs(3);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);
   And ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfConvertibleCompartmentAbnormalDefrostCycleCountJustOnceAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NumberOfConvertibleCompartmentAbnormalDefrostsIs(1);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes + 1);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfConvertibleCompartmentAbnormalDefrostCycleCountAndSetConvertibleCompartmentDefrostWasAbnormalWhenConvertibleCompartmentHeaterOnTimeReachesAbnormalTimeAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given NumberOfConvertibleCompartmentAbnormalDefrostsIs(1);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   Given LastConvertibleCompartmentDefrostWasNormal();
   Given NumberOfConvertibleCompartmentDefrostsIs(3);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);
   And ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldIncrementNumberOfConvertibleCompartmentAbnormalDefrostCycleCountJustOnceAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given NumberOfConvertibleCompartmentAbnormalDefrostsIs(1);
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes + 1);
   NumberOfConvertibleCompartmentAbnormalDefrostsShouldBe(2);
}

TEST(Defrost_TripleEvap, ShouldClearFreshFoodDefrostWasAbnormalAndFreshFoodDefrostHeaterOnForMaxTimeFaultWhenFreshFoodEvaporatorReachesTerminationTemperatureAndFreshFoodHeaterOnTimeIsLessThanFreshFoodHeaterOnTimeToSetAbnormalDefrostAndFreshFoodEvaporatorThermistorIsValidAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given LastFreshFoodDefrostWasAbnormal();
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenFreshFoodDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostWasAbnormalFlagShouldBe(CLEAR);
   FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_TripleEvap, ShouldNotClearFreshFoodDefrostWasAbnormalAndFreshFoodDefrostHeaterOnForMaxTimeFaultWhenFreshFoodEvaporatorReachesTerminationTemperatureAndFreshFoodHeaterOnTimeIsLessThanFreshFoodHeaterOnTimeToSetAbnormalDefrostAndFreshFoodEvaporatorThermistorIsInvalidAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodEvapThermistorValidityIs(Invalid);
   Given LastFreshFoodDefrostWasAbnormal();
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenFreshFoodDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostWasAbnormalFlagShouldBe(SET);
   FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldNotClearFreshFoodDefrostWasAbnormalWhenFreshFoodEvaporatorReachesTerminationTemperatureAndFreshFoodHeaterOnTimeIsEqualToFreshFoodHeaterOnTimeToSetAbnormalDefrostAndFreshFoodEvaporatorThermistorIsValidAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given LastFreshFoodDefrostWasAbnormal();
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldTurnOffFreshFoodDefrostHeaterAndIncrementFreshFoodDefrostCountAndTransitToDwellStateWhenFreshFoodEvaporatorTemperatureReachesFreshFoodDefrostTerminationTemperatureAndFreshFoodEvaporatorThermistorIsValidAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NumberOfFreshFoodDefrostsIs(1);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   And NumberOfFreshFoodDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndFreshFoodDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsValidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOnAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);
   GivenFreshFoodDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_TripleEvap, ShouldClearConvertibleCompartmentDefrostWasAbnormalAndConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenConvertibleCompartmentEvaporatorReachesTerminationTemperatureAndConvertibleCompartmentHeaterOnTimeIsLessThanConvertibleCompartmentHeaterOnTimeToSetAbnormalDefrostAndConvertibleCompartmentEvaporatorThermistorIsValidAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(CLEAR);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_TripleEvap, ShouldNotClearConvertibleCompartmentDefrostWasAbnormalAndConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenConvertibleCompartmentEvaporatorReachesTerminationTemperatureAndConvertibleCompartmentHeaterOnTimeIsLessThanConvertibleCompartmentHeaterOnTimeToSetAbnormalDefrostAndConvertibleCompartmentEvaporatorThermistorIsInvalidAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Invalid);
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(SET);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldNotClearConvertibleCompartmentDefrostWasAbnormalWhenConvertibleCompartmentEvaporatorReachesTerminationTemperatureAndConvertibleCompartmentHeaterOnTimeIsEqualToConvertibleCompartmentHeaterOnTimeToSetAbnormalDefrostAndConvertibleCompartmentEvaporatorThermistorIsValidAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldTurnOffConvertibleCompartmentDefrostHeaterAndIncrementConvertibleCompartmentDefrostCountAndTransitToDwellStateWhenConvertibleCompartmentEvaporatorTemperatureReachesConvertibleCompartmentDefrostTerminationTemperatureAndConvertibleCompartmentEvaporatorThermistorIsValidAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given NumberOfConvertibleCompartmentDefrostsIs(1);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   And NumberOfConvertibleCompartmentDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldNotClearConvertibleCompartmentDefrostWasAbnormalWhenConvertibleCompartmentEvaporatorReachesTerminationTemperatureAndConvertibleCompartmentHeaterOnTimeIsEqualToConvertibleCompartmentHeaterOnTimeToSetAbnormalDefrostAndConvertibleCompartmentEvaporatorThermistorIsValidAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenAllThermistorsAreValidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOnAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);
   GivenConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_TripleEvap, ShouldClearConvertibleCompartmentDefrostWasAbnormalAndConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenConvertibleCompartmentEvaporatorReachesTerminationTemperatureAndConvertibleCompartmentHeaterOnTimeIsLessThanConvertibleCompartmentHeaterOnTimeToSetAbnormalDefrostAndConvertibleCompartmentEvaporatorThermistorIsValidAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreshFood.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(CLEAR);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_TripleEvap, ShouldNotClearConvertibleCompartmentDefrostWasAbnormalAndConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenConvertibleCompartmentEvaporatorReachesTerminationTemperatureAndConvertibleCompartmentHeaterOnTimeIsLessThanConvertibleCompartmentHeaterOnTimeToSetAbnormalDefrostAndConvertibleCompartmentEvaporatorThermistorIsInvalidAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Invalid);
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(SET);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldNotClearConvertibleCompartmentDefrostWasAbnormalWhenConvertibleCompartmentEvaporatorReachesTerminationTemperatureAndConvertibleCompartmentHeaterOnTimeIsEqualToConvertibleCompartmentHeaterOnTimeToSetAbnormalDefrostAndConvertibleCompartmentEvaporatorThermistorIsValidAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   GivenConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeaterAsFreezer.heaterOnTimeToSetAbnormalDefrostInMinutes);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_TripleEvap, ShouldTurnOffConvertibleCompartmentDefrostHeaterAndIncrementConvertibleCompartmentDefrostCountAndTransitToDwellStateWhenConvertibleCompartmentEvaporatorTemperatureReachesConvertibleCompartmentDefrostTerminationTemperatureAndConvertibleCompartmentEvaporatorThermistorIsValidAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NumberOfConvertibleCompartmentDefrostsIs(1);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   And NumberOfConvertibleCompartmentDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultWhenAllThermistorsAreValidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOnAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);
   GivenConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   ConvertibleCompartmentDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_TripleEvap, ShouldNotTransitionToDwellUntilBothHeatersAreVotedOffAfterReachingTheirTerminationTemperaturesWithConvertibleCompartmentHeaterThenFreshFoodHeater)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);

   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFilteredConvertibleCompartmentEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldTurnOffDefrostHeatersAndIncrementFreshFoodDefrostCountAndTransitionToDwellStateWhenFreshFoodHeaterOnTimeReachesMaxOnTimeAndDefrostTypeIsSecondaryOnlyAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NumberOfFreezerDefrostsIs(1);
   Given NumberOfFreshFoodDefrostsIs(1);
   Given NumberOfConvertibleCompartmentDefrostsIs(1);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   And NumberOfFreezerDefrostsShouldBe(1);
   And NumberOfFreshFoodDefrostsShouldBe(2);
   And NumberOfConvertibleCompartmentDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldTurnOffDefrostHeatersAndIncrementFreshFoodAndFreezerDefrostCountsAndTransitionToDwellStateWhenFreshFoodHeaterOnTimeReachesMaxOnTimeAndDefrostTypeIsFullAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given NumberOfFreezerDefrostsIs(1);
   Given NumberOfFreshFoodDefrostsIs(1);
   Given NumberOfConvertibleCompartmentDefrostsIs(1);

   WhenConvertibleCompartmentDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   And NumberOfFreezerDefrostsShouldBe(2);
   And NumberOfFreshFoodDefrostsShouldBe(2);
   And NumberOfConvertibleCompartmentDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_TripleEvap, ShouldSkipPostDwellIfAnyPrimaryEvaporatorTempIsAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsFull)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_TripleEvap, ShouldSkipPostDwellIfAnyPrimaryEvaporatorTempIsAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsSecondary)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_TripleEvap, ShouldExitPostDwellIfAnyPrimaryEvaporatorTempChangesToAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsFull)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_TripleEvap, ShouldExitPostDwellIfAnyPrimaryEvaporatorTempChangesToAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsSecondaryOnly)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_TripleEvap, ShouldNotExitPostDwellWhenAnyNonPrimaryEvaporatorTempChangesToAtOrBelowPostDwellPrimaryEvapExitTemp)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredFreshFoodEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredConvertibleCompartmentEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_TripleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreezerEvapThermistorHasBeenDiscovered)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldNotGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidThenDiscoveredAndValidWhileInPrechillPrep)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreezerEvapThermistorHasNotBeenDiscovered();
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   WhenFreezerEvapThermistorHasBeenDiscovered();
   When FreezerEvaporatorThermistorValidityIs(Valid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldNotGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreezerEvapThermistorHasNotBeenDiscovered)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreezerEvapThermistorHasNotBeenDiscovered();
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldGoToHeaterOnEntryWhenFreshFoodEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreshFoodEvapThermistorHasBeenDiscovered)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenFreshFoodEvapThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldNotGoToHeaterOnEntryWhenFreshFoodEvapThermistorBecomesInvalidThenDiscoveredAndValidWhileInPrechillPrep)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreshFoodEvapThermistorHasNotBeenDiscovered();
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenFreshFoodEvapThermistorValidityIs(Invalid);
   WhenFreshFoodEvapThermistorHasBeenDiscovered();
   WhenFreshFoodEvapThermistorValidityIs(Valid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldNotGoToHeaterOnEntryWhenFreshFoodEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreshFoodEvapThermistorHasNotBeenDiscovered)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreshFoodEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenFreshFoodEvapThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldGoToHeaterOnEntryWhenConvertibleCompartmentEvapThermistorBecomesInvalidWhileInPrechillPrepAndConvertibleCompartmentEvapThermistorHasBeenDiscovered)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When ConvertibleCompartmentEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldNotGoToHeaterOnEntryWhenConvertibleCompartmentEvapThermistorBecomesInvalidThenDiscoveredAndValidWhileInPrechillPrep)
{
   Given ConvertibleCompartmentThermistorValidityIs(Valid);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   GivenConvertibleCompartmentEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When ConvertibleCompartmentEvaporatorThermistorValidityIs(Invalid);
   WhenConvertibleCompartmentEvapThermistorHasBeenDiscovered();
   When ConvertibleCompartmentEvaporatorThermistorValidityIs(Valid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldNotGoToHeaterOnEntryWhenConvertibleCompartmentEvapThermistorBecomesInvalidWhileInPrechillPrepAndConvertibleCompartmentEvapThermistorHasNotBeenDiscovered)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreshFoodEvapThermistorAndConvertibleCompartmentEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When ConvertibleCompartmentEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldSkipThroughPrechillPrepToHeaterOnEntryIfFreshFoodEvapThermistorIsInvalidAndHasBeenDiscoveredWhenEnteringPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   GivenFreshFoodEvapThermistorValidityIs(Invalid);
   GivenFreshFoodEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillPrepIfFreshFoodThermistorIsInvalidAndHasNotBeenDiscoveredWhenEnteringPrechillPrep)
{
   GivenAllThermistorsAreValidAndHaveBeenDiscoveredExceptForFreshFoodEvapThermistorHasNotBeenDiscovered();
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldSkipThroughPrechillPrepToHeaterOnEntryIfConvertibleCompartmentEvapThermistorIsInvalidAndHasBeenDiscoveredWhenEnteringPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   GivenFreshFoodThermistorHasBeenDiscovered();
   GivenConvertibleCompartmentThermistorValidityIs(Valid);
   GivenConvertibleCompartmentThermistorHasBeenDiscovered();
   GivenConvertibleCompartmentEvapThermistorValidityIs(Invalid);
   GivenConvertibleCompartmentEvapThermistorHasBeenDiscovered();

   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillPrepIfConvertibleCompartmentThermistorIsInvalidAndHasNotBeenDiscoveredWhenEnteringPrechillPrep)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   GivenFreshFoodThermistorHasBeenDiscovered();
   GivenConvertibleCompartmentThermistorValidityIs(Valid);
   GivenConvertibleCompartmentThermistorHasBeenDiscovered();
   GivenConvertibleCompartmentEvapThermistorValidityIs(Invalid);
   GivenConvertibleCompartmentEvapThermistorHasNotBeenDiscovered();

   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenCabinetAndEvapTemperaturesAreSuchThatPrechillConditionsAreNotMetYet();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndConvertibleCompartmentEvaporatorTemperatureIsBelowExitThresholdAndFreezerIsAboveWhileConvertibleCompartmentStateTypeIsFreezer)
{
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Valid);

   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);

   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);

   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   ConvertibleCompartmentThermistorValidityIs(Valid);

   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);

   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMetForTripleEvapAsConvertibleCompartment();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldGoToHeaterOnEntryWhenConvertibleCompartmentEvaporatorTemperatureChangesWhileInPrechillStateWhileConvertibleCompartmentStateTypeIsFreezer)
{
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   WhenPrechillConditionsAreMetForTripleEvapAsConvertibleCompartment();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   WhenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldRemainInPrechillStateWhenConvertibleCompartmentEvaporatorTemperatureChangesAndThermistorIsInvalidWhileInPrechillStateWhileConvertibleCompartmentStateTypeIsFreezer)
{
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   GivenConvertibleCompartmentEvapThermistorValidityIs(Invalid);
   GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   WhenPrechillConditionsAreMetForTripleEvapAsConvertibleCompartment();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   WhenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToHeaterOnEntryWhenConvertibleCompartmentEvaporatorTemperatureChangesToBeEqualToTheExitConditionsWhileInPrechillStateWhileConvertibleCompartmentStateTypeIsFreezer)
{
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   WhenPrechillConditionsAreMetForTripleEvapAsConvertibleCompartment();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldRemainInPrechillWhenConvertibleCompartmentEvaporatorTemperatureChangesToBeMoreThanTheExitConditionsWhileInPrechillStateWhileConvertibleCompartmentStateTypeIsFreezer)
{
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given GivenConvertibleCompartmentEvapThermistorValidityIs(Valid);
   Given GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   WhenPrechillConditionsAreMetForTripleEvapAsConvertibleCompartment();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When GivenFilteredConvertibleCompartmentEvapCabinetTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 10);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvaporatorTemperatureChangesWhileInPrechillStateWhileConvertibleCompartmentStateTypeIsFreshFood)
{
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvaporatorTemperatureChangesToBeEqualToTheExitConditionsWhileInPrechillStateWhileConvertibleCompartmentStateTypeIsFreshFood)
{
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_TripleEvap, ShouldRemainInPrechillWhenFreezerEvaporatorTemperatureChangesToBeMoreThanTheExitConditionsWhileInPrechillStateWhileConvertibleCompartmentStateTypeIsFreshFood)
{
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 10);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}
