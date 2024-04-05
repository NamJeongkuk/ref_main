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
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 0,
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet = 0
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

TEST_GROUP(Defrost_SingleEvapAllFreezer)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;
   EnhancedSabbathData_t enhancedSabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleDoorSingleEvapAllFreezer);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      platformData.numberOfEvaporators = 1;

      enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 0;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
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

   void ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void WhenFreezerDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void GivenFreezerDefrostHeaterMaxOnTimeInMinutesIs(uint8_t timeInMinutes)
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

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
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

   void CalculatedGridLinesAre(TwoDimensionalCalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &gridLines);
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

   void WhenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }
};

TEST(Defrost_SingleEvapAllFreezer, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldGoToPrechillWhenEnteringPrechillPrepAndCompressorIsOnAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldGoToPrechillWhenCompressorTurnsOnWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldExitOnHeaterEntryStateAndTurnOnTheFreezerDefrostHeaterAfterDefrostHeaterOnDelayTimerExpired)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvapAllFreezer, ShouldSetInvalidEvaporatorThermistorDuringDefrostAndNotSetFreezerDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsInvalidAndHasBeenDiscoveredAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   GivenFreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);

   WhenFreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(SET);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndSetFreezerDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsValidAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   GivenFreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);

   WhenFreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldTransitionFromHeaterOnToDwellAfterFreezerHeaterIsOnForMaxTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   GivenFreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Valid);

   WhenFreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitionToDwellStateWhenFreezerHeaterOnTimeReachesMaxOnTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerDefrostsIs(1);
   Given NumberOfFreshFoodDefrostsIs(1);
   Given NumberOfConvertibleCompartmentDefrostsIs(1);
   GivenFreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes - 1);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   WhenFreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   And NumberOfFreezerDefrostsShouldBe(2);
   And NumberOfFreshFoodDefrostsShouldBe(1);
   And NumberOfConvertibleCompartmentDefrostsShouldBe(1);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldSkipPostDwellIfAnyPrimaryEvaporatorTempIsAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsFull)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldSkipPostDwellIfAnyPrimaryEvaporatorTempIsAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsSecondary)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldOnlyExitPostDwellWhenFreezerEvaporatorTempChangesToAtOrBelowPostDwellPrimaryEvapExitTemp)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredFreshFoodEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredFreezerEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreezerEvapThermistorHasBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldNotGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidThenDiscoveredAndValidWhileInPrechillPrep)
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

TEST(Defrost_SingleEvapAllFreezer, ShouldNotGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreezerEvapThermistorHasNotBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldSkipThroughPrechillPrepToHeaterOnEntryIfFreezerEvapThermistorIsInvalidAndHasBeenDiscoveredWhenEnteringPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldGoToPrechillPrepIfFreezerThermistorIsInvalidAndHasNotBeenDiscoveredWhenEnteringPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldGoToHeaterOnEntryWhenPrimaryEvaporatorTemperatureChangesToMeetExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldGoToHeaterOnEntryWhenPrimaryEvaporatorTemperatureChangesToBeEqualToTheExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldRemainInPrechillWhenPrimaryEvaporatorTemperatureChangesToBeMoreThanTheExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 10);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}
