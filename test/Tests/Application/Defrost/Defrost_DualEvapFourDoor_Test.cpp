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

TEST_GROUP(Defrost_DualEvapFourDoor)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;
   EnhancedSabbathData_t enhancedSabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapFourDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_FreshFood);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      platformData.numberOfEvaporators = 2;

      enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 3;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfiguration, &defrostData, &platformData, &enhancedSabbathData);
   }

   void GivenDefrostIsInitializedWithEepromNotClearedOnStartup()
   {
      Defrost_Init(&instance, dataModel, &defrostConfiguration, &defrostData, &platformData, &enhancedSabbathData);
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
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_Freezer);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      DefrostType_t currentDefrostType;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &currentDefrostType);

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

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
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

   void GivenFilteredFreshFoodEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void GivenFreshFoodEvapThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvapThermistor_IsValidResolved, &state);
   }

   void WhenFilteredFreshFoodEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenFilteredFreshFoodEvapTemperatureIs(temperature);
   }

   void CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_CabinetFilteredTemperatureTooWarmOnPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void GivenFreezerThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, set);
   }

   void GivenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
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

   void FreezerDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void FreezerDefrostHeaterMaxOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterMaxOnTimeInMinutes, &timeInMinutes);
   }

   void InvalidEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidEvaporatorThermistorDuringDefrost, &state);
   }

   void InvalidEvaporatorThermistorDuringDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_InvalidEvaporatorThermistorDuringDefrost, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeaterOnForMaxTimeFault, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenFreshFoodEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvaporatorThermistorDiscovered, set);
   }

   void GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(timeInMinutes);
   }

   void GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes, &timeInMinutes);
   }

   void GivenFreshFoodDefrostHeaterOnForMaxTimeFaultIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeaterOnForMaxTimeFault, &state);
   }

   void FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeaterOnForMaxTimeFault, &actual);
      CHECK_EQUAL(expected, actual);
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

   void LastFreshFoodDefrostWasAbnormal()
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, set);
   }

   void FreshFoodDefrostWasAbnormalFlagShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostWasAbnormal, &actual);

      CHECK_EQUAL(expected, actual);
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

   void GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_t type)
   {
      Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
      if(type == DefrostType_Full)
      {
         GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
      }
      Given CurrentDefrostTypeIs(type);
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

   void GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      GivenConvertibleCompartmentStateIs(state);
   }

   void GivenFreshFoodEvapThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvaporatorThermistorDiscovered, clear);
   }

   void WhenFreshFoodEvapThermistorValidityIs(bool state)
   {
      GivenFreshFoodEvapThermistorValidityIs(state);
   }

   void GivenFreezerEvapThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, clear);
   }

   void WhenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }

   void WhenFreshFoodEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvaporatorThermistorDiscovered, set);
   }
};

TEST(Defrost_DualEvapFourDoor, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCoolingModeIsFreshFoodAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToPrechillWhenCompressorTurnsOnAndPrechillConditionsMetWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_Freezer);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToPrechillWhenCoolingModeChangesToFreezerAndPrechillConditionsMetWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When CoolingModeIs(CoolingMode_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpErdToFalseWhenFreshFoodEvapFilteredTemperatureIsAtDefrostTerminationTemperatureAndThermistorIsValid)
{
   GivenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNextDefrostTypeOverrideToFullIfEepromClearedOnPowerUp)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   GivenDefrostIsInitializedWithEepromClearedOnStartup();

   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNextDefrostTypeOverrideToFullIfCabinetTooWarmOnPowerUp)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitialized();

   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNextDefrostTypeOverrideToFullIfDefrostWasDisabledBeforePowerUp)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitialized();

   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetCurrentDefrostTypeToSecondaryOnlyDefrostOnIdleExitWhenNextDefrostTypeOverrideIsDontCareAndNumberOfSecondaryDefrostsIsLessThanNumberOfSecondaryOnlyDefrostsBeforeFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost - 1);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetCurrentDefrostTypeToFullOnIdleExitWhenNextDefrostTypeOverrideIsDontCareAndNumberOfSecondaryDefrostIsGreaterThanNumberOfSecondaryOnlyDefrostsBeforeFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost + 1);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetCurrentDefrostTypeToFullOnIdleExitWhenNextDefrostTypeOverrideIsDontCareAndNumberOfSecondaryDefrostIsEqualToNumberOfSecondaryOnlyDefrostsBeforeFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetCurrentDefrostTypeToNextDefrostTypeOverrideOnIdleExitWhenNextDefrostTypeIsSecondaryOnly)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNextDefrostTypeOverrideToDontCareOnIdleExitWhenNextDefrostTypeIsSecondaryOnly)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NextDefrostTypeOverrideShouldBe(DefrostType_DontCare);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetCurrentDefrostTypeToNextDefrostTypeOverrideAndIncrementNumberOfSecondaryDefrostsOnIdleExitWhenNextDefrostTypeIsSecondaryOnlyOnIdleExitViaPrechillTestRequest)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenNumberOfSecondaryOnlyDefrostsIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenIdleIsExitedVia(DefrostTestStateRequest_Prechill);
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
   NumberOfSecondaryOnlyDefrostsShouldBe(1);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetCurrentDefrostTypeToNextDefrostTypeOverrideAndIncrementNumberOfSecondaryDefrostsOnIdleExitWhenNextDefrostTypeIsSecondaryOnlyOnIdleExitViaDefrostTestRequest)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenNumberOfSecondaryOnlyDefrostsIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenIdleIsExitedVia(DefrostTestStateRequest_Defrost);
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
   NumberOfSecondaryOnlyDefrostsShouldBe(1);
}

TEST(Defrost_DualEvapFourDoor, ShouldIncrementNumberOfSecondaryOnlyDefrostsOnIdleExitWhenNextDefrostTypeIsSecondaryOnlyOnIdleExit)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   GivenNumberOfSecondaryOnlyDefrostsIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsShouldBe(1);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNumberOfSecondaryOnlyDefrostsToZeroOnIdleExitWhenNextDefrostTypeIsNotSecondaryOnlyOnIdleExit)
{
   Given NextDefrostTypeOverrideIs(DefrostType_Full);
   GivenNumberOfSecondaryOnlyDefrostsIs(10);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsShouldBe(0);
}

TEST(Defrost_DualEvapFourDoor, ShouldIncrementNumberOfSecondaryOnlyDefrostOnIdleExitWhenNextDefrostTypeIsDontCareAndNumberOfSecondaryDefrostIsLessThanNumberOfSecondaryOnlyDefrostsBeforeFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(0);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsShouldBe(1);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNumberOfSecondaryOnlyDefrostToZeroOnIdleExitWhenNextDefrostTypeIsDontCareAndNumberOfSecondaryDefrostIsGreaterThanNumberOfSecondaryOnlyDefrostsBeforeFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsShouldBe(0);
}

TEST(Defrost_DualEvapFourDoor, ShouldSkipDefrostTypeDeterminationOnIdleExitWhenDefrostIsDisabled)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DisableDefrostIs(ENABLED);
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotSkipDefrostTypeDeterminationOnIdleExitWhenDefrostIsDisabledAndThenEnabled)
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

TEST(Defrost_DualEvapFourDoor, ShouldSetNumberOfSecondaryOnlyDefrostsOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNumberOfSecondaryOnlyDefrostsToEnhancedSabbathDataWhenEnhancedSabbathIsEnabledOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenEnhancedSabbathModeIs(ENABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNumberOfSecondaryOnlyDefrostsToEnhancedSabbathDataWhenSabbathIsEnabledOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenSabbathModeIs(ENABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetNumberOfSecondaryOnlyDefrostsToIdleAbnormalDataWhenFreezerDefrostWasAbnormalOnInit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenLastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(Defrost_DualEvapFourDoor, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenEnhancedSabbathModeIsEnabledOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenEnhancedSabbathModeIs(DISABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenEnhancedSabbathModeIs(ENABLED);
   And WhenSabbathIsReadyToDefrost();
   And WhenEnhancedSabbathIsRequestingToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_DualEvapFourDoor, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenEnhancedSabbathModeIsDisabledOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenEnhancedSabbathModeIs(ENABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenEnhancedSabbathModeIs(DISABLED);
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_DualEvapFourDoor, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenSabbathModeIsEnabledOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenSabbathModeIs(DISABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenSabbathModeIs(ENABLED);
   And WhenSabbathIsReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_DualEvapFourDoor, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenSabbathModeIsDisabledOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenEnhancedSabbathModeIs(ENABLED);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenEnhancedSabbathModeIs(DISABLED);
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_DualEvapFourDoor, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenFreezerDefrostIsAbnormalOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenLastFreezerDefrostWasNormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenLastFreezerDefrostWasAbnormal();
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(Defrost_DualEvapFourDoor, ShouldUpdateNumberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenFreezerDefrostIsNormalOnIdleExit)
{
   GivenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIs(10);
   GivenLastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenLastFreezerDefrostWasNormal();
   And ReadyToDefrost();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(Defrost_DualEvapFourDoor, ShouldExitOnHeaterEntryStateAndTurnOnTheFreshFoodAndFreezerDefrostHeaterAfterDefrostHeaterOnDelayTimerExpiredWhenDefrostTypeIsFull)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   Given CurrentDefrostTypeIs(DefrostType_Full);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();
}

TEST(Defrost_DualEvapFourDoor, ShouldExitOnHeaterEntryStateAndTurnOnTheFreezerDefrostHeaterAfterDefrostHeaterOnDelayTimerExpiredWhenDefrostTypeIsSecondaryOnly)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);

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

TEST(Defrost_DualEvapFourDoor, ShouldSetInvalidEvaporatorThermistorDuringDefrostAndNotSetFreezerDefrostHeaterOnForMaxTimeFaultWhenTheFreezerEvapThermistorIsInvalidAndHasBeenDiscoveredAndMaxOnTimeReachesOnHeaterOn)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(SET);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_DualEvapFourDoor, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndSetFreezerDefrostHeaterOnForMaxTimeFaultWhenFreshFoodEvapAndFreezerEvapThermistorsAreValidAndMaxOnTimeReachesOnHeaterOn)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_DualEvapFourDoor, ShouldSetInvalidEvaporatorThermistorDuringDefrostAndNotSetFreshFoodDefrostHeaterOnForMaxTimeFaultWhenTheFreshFoodThermistorIsInvalidAndHasBeenDiscoveredAndMaxOnTimeReachesOnHeaterOn)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodEvapThermistorHasBeenDiscovered();
   GivenFreshFoodEvapThermistorValidityIs(Invalid);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(SET);
   FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_DualEvapFourDoor, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndSetFreshFoodDefrostHeaterOnForMaxTimeFaultWhenFreshFoodEvapAndFreezerEvapThermistorsAreValidAndMaxOnTimeReachesOnHeaterOn)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_DualEvapFourDoor, ShouldTransitionFromHeaterOnToDwellAfterFreshFoodHeaterIsOnForMaxTimeWhenDefrostTypeIsSecondaryOnly)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodEvapThermistorValidityIs(Valid);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_DualEvapFourDoor, ShouldIncrementNumberOfFreshFoodAbnormalDefrostCycleCountAndSetFreshFoodDefrostWasAbnormalWhenFreshFoodHeaterOnTimeReachesAbnormalTime)
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

TEST(Defrost_DualEvapFourDoor, ShouldIncrementNumberOfFreshFoodAbnormalDefrostCycleCountJustOnce)
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

TEST(Defrost_DualEvapFourDoor, ShouldClearFreshFoodDefrostWasAbnormalAndFreshFoodDefrostHeaterOnForMaxTimeFaultWhenFreshFoodEvaporatorReachesTerminationTemperatureAndFreshFoodHeaterOnTimeIsLessThanFreshFoodHeaterOnTimeToSetAbnormalDefrostAndFreshFoodEvaporatorThermistorIsValid)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given LastFreshFoodDefrostWasAbnormal();
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenFreshFoodDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostWasAbnormalFlagShouldBe(CLEAR);
   FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotClearFreshFoodDefrostWasAbnormalAndFreshFoodDefrostHeaterOnForMaxTimeFaultWhenFreshFoodEvaporatorReachesTerminationTemperatureAndFreshFoodHeaterOnTimeIsLessThanFreshFoodHeaterOnTimeToSetAbnormalDefrostAndFreshFoodEvaporatorThermistorIsInvalid)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenFreshFoodEvapThermistorValidityIs(Invalid);
   Given LastFreshFoodDefrostWasAbnormal();
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenFreshFoodDefrostHeaterOnForMaxTimeFaultIs(SET);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostWasAbnormalFlagShouldBe(SET);
   FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotClearFreshFoodDefrostWasAbnormalWhenFreshFoodEvaporatorReachesTerminationTemperatureAndFreshFoodHeaterOnTimeIsEqualToFreshFoodHeaterOnTimeToSetAbnormalDefrostAndFreshFoodEvaporatorThermistorIsValid)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given LastFreshFoodDefrostWasAbnormal();
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.heaterOnTimeToSetAbnormalDefrostInMinutes);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_DualEvapFourDoor, ShouldTurnOffFreshFoodDefrostHeaterWhenFreshFoodEvaporatorTemperatureReachesFreshFoodDefrostTerminationTemperatureAndFreshFoodEvaporatorThermistorIsValid)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given NumberOfFreshFoodDefrostsIs(1);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
}

TEST(Defrost_DualEvapFourDoor, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndFreshFoodDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsValidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOn)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);
   GivenFreshFoodDefrostHeaterOnForMaxTimeFaultIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);

   WhenFilteredFreshFoodEvapTemperatureIs(defrostData.heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreshFoodDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_DualEvapFourDoor, ShouldTurnOffDefrostHeaterAndIncrementFreshFoodDefrostCountAndTransitionToDwellStateWhenFreshFoodHeaterOnTimeReachesMaxOnTimeAndDefrostTypeIsSecondaryOnly)
{
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NumberOfFreezerDefrostsIs(1);
   Given NumberOfFreshFoodDefrostsIs(1);
   Given NumberOfConvertibleCompartmentDefrostsIs(1);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes - 1);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   And NumberOfFreezerDefrostsShouldBe(1);
   And NumberOfFreshFoodDefrostsShouldBe(2);
   And NumberOfConvertibleCompartmentDefrostsShouldBe(1);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_DualEvapFourDoor, ShouldTurnOffDefrostHeaterAndIncrementFreshFoodAndFreezerDefrostCountsAndTransitionToDwellStateWhenFreshFoodHeaterOnTimeReachesMaxOnTimeAndDefrostTypeIsFull)
{
   GivenNumberOfSecondaryOnlyDefrostsIs(defrostData.idleData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
   GivenDefrostIsInHeaterOnAndDefrostTypeIs(DefrostType_Full);
   Given NumberOfFreezerDefrostsIs(1);
   Given NumberOfFreshFoodDefrostsIs(1);
   Given NumberOfConvertibleCompartmentDefrostsIs(1);
   GivenFreshFoodDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes - 1);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   WhenFreshFoodDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

   And NumberOfFreezerDefrostsShouldBe(2);
   And NumberOfFreshFoodDefrostsShouldBe(2);
   And NumberOfConvertibleCompartmentDefrostsShouldBe(1);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_DualEvapFourDoor, ShouldSkipPostDwellIfAnyPrimaryEvaporatorTempIsAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsFull)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_DualEvapFourDoor, ShouldSkipPostDwellIfAnyPrimaryEvaporatorTempIsAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsSecondary)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_DualEvapFourDoor, ShouldOnlyExitPostDwellWhenFreshFoodEvaporatorTempChangesToAtOrBelowPostDwellPrimaryEvapExitTemp)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenAllThermistorsValidityIs(Valid);
   GivenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredFreshFoodEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredConvertibleCompartmentEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredFreezerEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreezerEvapThermistorHasBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidThenDiscoveredAndValidWhileInPrechillPrep)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   WhenFreezerEvapThermistorHasBeenDiscovered();
   When FreezerEvaporatorThermistorValidityIs(Valid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreezerEvapThermistorHasNotBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToHeaterOnEntryWhenFreshFoodEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreshFoodEvapThermistorHasBeenDiscovered)
{
   Given FreshFoodThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenFreshFoodEvapThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotGoToHeaterOnEntryWhenFreshFoodEvapThermistorBecomesInvalidThenDiscoveredAndValidWhileInPrechillPrep)
{
   Given FreshFoodThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenFreshFoodEvapThermistorValidityIs(Invalid);
   WhenFreshFoodEvapThermistorHasBeenDiscovered();
   WhenFreshFoodEvapThermistorValidityIs(Valid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotGoToHeaterOnEntryWhenFreshFoodEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreshFoodEvapThermistorHasNotBeenDiscovered)
{
   Given FreshFoodThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenFreshFoodEvapThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldSkipThroughPrechillPrepToHeaterOnEntryIfFreshFoodEvapThermistorIsInvalidAndHasBeenDiscoveredWhenEnteringPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Invalid);
   GivenFreshFoodEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToPrechillPrepIfFreshFoodThermistorIsInvalidAndHasNotBeenDiscoveredWhenEnteringPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   GivenFreshFoodEvapThermistorValidityIs(Invalid);
   GivenFreshFoodEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToHeaterOnEntryWhenPrimaryEvaporatorTemperatureChangesToMeetExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToHeaterOnEntryWhenPrimaryEvaporatorTemperatureChangesToBeEqualToTheExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_DualEvapFourDoor, ShouldRemainInPrechillWhenPrimaryEvaporatorTemperatureChangesToBeMoreThanTheExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 10);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}
