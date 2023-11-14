/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ReadyToDefrost.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Closed = 0,
   Open = 1,
   Opened = Open
};

enum
{
   SomeCompressorOnTimeInSeconds = 400,
   SomeDoorAccelerationsInSecondsPerSecond = 200
};

enum
{
   SomeSeconds = 20,
   UpdatePeriodInSeconds = 1 * SECONDS_PER_MINUTE,
   LessThanPeriodicUpdateInSeconds = UpdatePeriodInSeconds - 3,
};

static ReadyToDefrostDoorConfiguration_t doorsConfiguration[] = {
   {
      .doorIsOpenErd = Erd_LeftSideFreshFoodDoorIsOpenResolved,
      .doorAccelerationErd = Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freshFoodDoorIncrementFactorInSecondsPerSecond),
   },
   {
      .doorIsOpenErd = Erd_LeftSideFreezerDoorStatusResolved,
      .doorAccelerationErd = Erd_FreezerScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freezerDoorIncrementFactorInSecondsPerSecond),
   },
   {
      .doorIsOpenErd = Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen,
      .doorAccelerationErd = Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds,
      .offsetInParametricForDoorFactor = OFFSET_OF(DefrostData_t, idleData.freshFoodDoorIncrementFactorInSecondsPerSecond),
   },
};

static ReadyToDefrostConfiguration_t config = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .defrostCompressorOnTimeInSecondsErd = Erd_DefrostCompressorOnTimeInSeconds,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUp,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,
   .invalidFreezerEvaporatorThermistorDuringDefrostErd = Erd_InvalidFreezerEvaporatorThermistorDuringDefrost,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .eepromClearedErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .waitingForDefrostTimeInSecondsErd = Erd_WaitingForDefrostTimeInSeconds,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .timeBetweenDefrostsInMinutesErd = Erd_TimeBetweenDefrostsInMinutes,
   .readyToDefrostHsmStateErd = Erd_ReadyToDefrostHsmState,
   .doorsConfiguration = doorsConfiguration,
   .numberOfDoors = NUM_ELEMENTS(doorsConfiguration)
};

static const DefrostIdleData_t idleData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 10 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 96 * MINUTES_PER_HOUR,
   .ahamPrechillTimeBetweenDefrostsInMinutes = 6 * MINUTES_PER_HOUR
};

static const DefrostIdleData_t shorterTimeBetweenDefrostsIdleData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 3 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 5 * MINUTES_PER_HOUR,
   .ahamPrechillTimeBetweenDefrostsInMinutes = 3 * MINUTES_PER_HOUR
};

static const DefrostPrechillPrepData_t prechillPrepData = {
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 0,
   .numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost = 0,
   .maxPrechillPrepTimeInMinutes = 5
};

static const DefrostPrechillData_t prechillData = {
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
   .prechillRefrigerantValvePosition = ValvePosition_B,
   .prechillCompressorSpeed = CompressorSpeed_Low,
   .prechillFreezerEvapFanSpeed = FanSpeed_SuperLow,
   .prechillFreshFoodEvapFanSpeed = FanSpeed_Low,
   .prechillIceBoxFanSpeed = FanSpeed_Medium,
   .prechillDeliFanSpeed = FanSpeed_High,
   .prechillConvertibleCompartmentDamperPosition = DamperPosition_Open,
   .prechillConvertibleCompartmentFanSpeed = FanSpeed_High,
   .prechillFreshFoodDamperPosition = DamperPosition_Closed,
   .prechillFreezerMinTempInDegFx100 = -600,
   .prechillFreshFoodMinTempInDegFx100 = 3200,
   .prechillFreshFoodMaxTempInDegFx100 = 4600,
   .prechillConvertibleCompartmentAsFreezerMinTempInDegFx100 = -600,
   .prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 = 3200,
   .prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100 = 4600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000
};

static const DefrostHeaterOnEntryData_t heaterOnEntryData = {
   .defrostHeaterOnDelayAfterCompressorOffInSeconds = 2,
   .heaterOnEntryFreshFoodDamperPosition = DamperPosition_Open
};

static const DefrostHeaterOnData_t heaterOnData = {
   .defrostHeaterOnRefrigerantValvePosition = ValvePosition_A,
   .freezerDefrostHeaterMaxOnTimeInMinutes = 60,
   .freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .freezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
   .freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20,
   .freshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60,
   .convertibleCompartmentAsFreshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21,
   .convertibleCompartmentAsFreezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 35,
   .convertibleCompartmentAsFreshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 2,
   .convertibleCompartmentAsFreezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 25
};

static const DefrostDwellData_t dwellData = {
   .dwellTimeInMinutes = 7,
   .dwellFreshFoodDamperPosition = DamperPosition_Closed,
   .dwellRefrigerantValvePosition = ValvePosition_B
};

static const DefrostPostDwellData_t postDwellData = {
   .postDwellExitTimeInMinutes = 5,
   .freshFoodOnlyPostDwellExitTimeInMinutes = 10,
   .postDwellFreezerEvapExitTemperatureInDegFx100 = -1000,
   .freshFoodOnlyPostDwellFreezerEvapExitTemperatureInDegFx100 = -1200,
   .postDwellRefrigerantValvePosition = ValvePosition_B,
   .postDwellCompressorSpeed = CompressorSpeed_Low,
   .postDwellCondenserFanSpeed = FanSpeed_Low,
   .postDwellFreshFoodDamperPosition = DamperPosition_Closed
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

static const DefrostData_t shorterTimeBetweenDefrostData = {
   .idleData = shorterTimeBetweenDefrostsIdleData,
   .prechillPrepData = prechillPrepData,
   .prechillData = prechillData,
   .heaterOnEntryData = heaterOnEntryData,
   .heaterOnData = heaterOnData,
   .dwellData = dwellData,
   .postDwellData = postDwellData
};

TEST_GROUP(ReadyToDefrost)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   ReadyToDefrost_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
   }

   void GivenReadyToDefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);

      ReadyToDefrost_Init(&instance, dataModel, &config, &defrostData);
   }

   void GivenReadyToDefrostIsInitializedWithLesserTimeBetweenDefrostsInParametric()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);

      OldMinimumTimeBetweenDefrostsShouldBeGreaterThanNewMaximumTimeBetweenDefrosts();
      ReadyToDefrost_Init(&instance, dataModel, &config, &shorterTimeBetweenDefrostData);
   }

   void ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_t expected)
   {
      ReadyToDefrostHsmState_t actual;
      DataModel_Read(dataModel, Erd_ReadyToDefrostHsmState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenWaitingToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, &state);
   }

   void WhenWaitingToDefrostIs(bool state)
   {
      GivenWaitingToDefrostIs(state);
   }

   void WhenDefrostChangesFromWaitingToDefrostToDefrostingBackToWaitingToDefrost()
   {
      WhenWaitingToDefrostIs(false);
      // cleared by defrost HSM
      WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
      WhenWaitingToDefrostIs(true);
   }
   void GivenFreezerTooWarmOnPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUp, &state);
   }

   void WhenFreezerTooWarmAtPowerUpChangesTo(bool state)
   {
      GivenFreezerTooWarmOnPowerUpIs(state);
   }

   void GivenCompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void WhenCompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      GivenCompressorOnTimeInSecondsIs(seconds);
   }

   void CompressorOnTimeInSecondsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void GivenDoorAccelerationsAre(uint32_t seconds)
   {
      for(uint8_t i = 0; i < config.numberOfDoors; i++)
      {
         DataModel_Write(
            dataModel,
            config.doorsConfiguration[i].doorAccelerationErd,
            &seconds);
      }
   }

   void DoorAccelerationsShouldBe(uint32_t expected)
   {
      for(uint8_t i = 0; i < config.numberOfDoors; i++)
      {
         uint32_t actual;
         DataModel_Read(
            dataModel,
            config.doorsConfiguration[i].doorAccelerationErd,
            &actual);

         CHECK_EQUAL(expected, actual);
      }
   }

   void GivenFreshFoodDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void WhenFreshFoodDefrostWasAbnormalChangesTo(bool state)
   {
      GivenFreshFoodDefrostWasAbnormalIs(state);
   }

   void GivenFreezerDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void WhenFreezerDefrostWasAbnormalChangesTo(bool state)
   {
      GivenFreezerDefrostWasAbnormalIs(state);
   }

   void GivenConvertibleCompartmentDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void WhenConvertibleCompartmentDefrostWasAbnormalChangesTo(bool state)
   {
      GivenConvertibleCompartmentDefrostWasAbnormalIs(state);
   }

   void GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidFreezerEvaporatorThermistorDuringDefrost, &state);
   }

   void WhenInvalidFreezerEvaporatorThermistorDuringDefrostChangesTo(bool state)
   {
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(state);
   }

   void GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(bool state)
   {
      DataModel_Write(dataModel, Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts, &state);
   }

   void WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(bool state)
   {
      GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(state);
   }

   void GivenEepromClearedFlagIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &state);
   }

   void WhenEepromClearedFlagChangesTo(bool state)
   {
      GivenEepromClearedFlagIs(state);
   }

   void TimeBetweenDefrostsInMinutesShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_TimeBetweenDefrostsInMinutes, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenItHasAConvertibleCompartmentIs(bool state)
   {
      DataModel_Write(dataModel, Erd_HasConvertibleCompartment, &state);
   }

   void WaitingDefrostTimeInSecondsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_WaitingForDefrostTimeInSeconds, &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void ReadyToDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ReadyToDefrost, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenCompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void WhenCompressorTurns(bool state)
   {
      GivenCompressorIs(state);
   }

   void GivenLeftSideFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorIsOpenResolved, &state);
   }

   void GivenRightSideFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatusResolved, &state);
   }

   void GivenDoorInDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DoorInDoorIsOpenResolved, &state);
   }

   void GivenFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatusResolved, &state);
   }

   void GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen, &state);
   }

   void GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal()
   {
      GivenFreshFoodDefrostWasAbnormalIs(false);
      GivenFreezerDefrostWasAbnormalIs(true);
      GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenFreezerTooWarmOnPowerUpIs(false);
      GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
      GivenEepromClearedFlagIs(false);
   }

   void GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum()
   {
      GivenFreshFoodDefrostWasAbnormalIs(false);
      GivenFreezerDefrostWasAbnormalIs(false);
      GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
      GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
      GivenFreezerTooWarmOnPowerUpIs(false);
      GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
      GivenEepromClearedFlagIs(false);
   }

   void GivenCompressorOnTimeIsZeroAndTimeBetweenDefrostsIsMinimumAndSystemIsInWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenCompressorOnTimeInSecondsIs(0);
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();

      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   }

   void GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsFalse()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsTrue()
   {
      GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized()
   {
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenWaitingToDefrostIsFalseAndReadyToDefrostIsInitialized()
   {
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();
   }

   void WhenLeftSideFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorIsOpenResolved, &state);
   }

   void WhenFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatusResolved, &state);
   }

   void GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(
         dataModel,
         Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds,
         &seconds);
   }

   void WhenLeftSideFreshFoodDoorAccelerationInSecondsIs(uint32_t seconds)
   {
      GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(seconds);
   }

   void LeftSideFreshFoodDoorAccelerationsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(
         dataModel,
         Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds,
         &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void GivenFreezerDoorAccelerationInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(
         dataModel,
         Erd_FreezerScaledDoorAccelerationInSeconds,
         &seconds);
   }

   void FreezerDoorAccelerationsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(
         dataModel,
         Erd_FreezerScaledDoorAccelerationInSeconds,
         &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(
         dataModel,
         Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds,
         &actual);

      CHECK_TRUE((actual <= (expected + expected / 100)) && (actual >= (expected - expected / 100)));
   }

   void WhenConvertibleCompartmentAsFreshFoodDoorIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_ConvertibleCompartmentAsFreshFoodDoorIsOpen,
         &state);
   }

   void GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsBelowMinimumByOneSecondAndWaitingToDefrostIsTrue()
   {
      GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
   }

   void GivenTimeBetweenDefrostsIsMinimumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsFalseAndStateIsReadyAndDefrosting()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
      GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();

      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   }

   void GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsFalseAndStateIsReadyAndDefrosting()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      GivenWaitingToDefrostIs(false);
      GivenReadyToDefrostIsInitialized();

      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   }

   void GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
      GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
      GivenWaitingToDefrostIs(true);
      GivenReadyToDefrostIsInitialized();
      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Closed);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Closed);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftSideFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Closed);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Closed);
      GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();
   }

   void WhenCompressorTurnsOffAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenCompressorTurns(OFF);
   }

   void WhenCompressorTurnsOnAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenCompressorTurns(ON);
   }

   void WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenLeftSideFreshFoodDoorIs(Open);
   }

   void WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenLeftSideFreshFoodDoorIs(Closed);
   }

   void WhenConvertibleCompartmentAsFreshFoodDoorClosesAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenConvertibleCompartmentAsFreshFoodDoorIs(Closed);
   }

   void WhenFreezerDoorOpensAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenFreezerDoorIs(Open);
   }

   void WhenFreezerDoorClosesAfterSomeSeconds()
   {
      After(SomeSeconds * MSEC_PER_SEC);
      WhenFreezerDoorIs(Closed);
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentAsFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Closed);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentAsFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Closed);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Closed);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts()
   {
      GivenLeftSideFreshFoodDoorIs(Closed);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Open);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOffAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftSideFreshFoodDoorIs(Open);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(OFF);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftSideFreshFoodDoorIs(Closed);
      GivenFreezerDoorIs(Open);
      GivenCompressorIs(ON);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenLeftSideFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState()
   {
      GivenLeftSideFreshFoodDoorIs(Closed);
      GivenFreezerDoorIs(Open);
      GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();
   }

   void GivenCompressorIsOnForMinimumTimeAndUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueOneMillisecondBeforeTheMinimumTime()
   {
      GivenCompressorIs(ON);
      GivenCompressorOnTimeInSecondsIs(0);
      GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

      After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
      ReadyToDefrostShouldBe(false);

      WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   }
   void GivenLeftSideFreshFoodDoorHasBeenOpenForTenPeriodicUpdatePeriodsAndCompressorIsOff()
   {
      GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

      WhenLeftSideFreshFoodDoorIs(Open);

      for(uint8_t i = 0; i < 10; i++)
      {
         After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
         LeftSideFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

         After(1);
         LeftSideFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      }
   }

   void CompressorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorIsOn, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenCompressorIsOnForOneUpdatePeriod()
   {
      CompressorShouldBe(ON);
      After(UpdatePeriodInSeconds * MSEC_PER_SEC);
   }

   void WhenCompressorIsOffForOneUpdatePeriod()
   {
      WhenCompressorTurns(OFF);
      After(UpdatePeriodInSeconds * MSEC_PER_SEC);
   }

   void OldMinimumTimeBetweenDefrostsShouldBeGreaterThanNewMaximumTimeBetweenDefrosts()
   {
      CHECK_TRUE(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes > shorterTimeBetweenDefrostData.idleData.maxTimeBetweenDefrostsInMinutes);
   }
};

TEST(ReadyToDefrost, ShouldInitialize)
{
   GivenReadyToDefrostIsInitialized();
}

TEST(ReadyToDefrost, ShouldInitializeIntoWaitingForMinimumTimeBetweenDefrostsWhenSystemIsWaitingToDefrostAndCompressorOnTimeIsLessThanMinimumTimeBetweenDefrostsOnInit)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMinimumBecauseFreezerDefrostWasAbnormal();
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldInitializeIntoWaitingForRemainingTimeBetweenDefrostsWhenSystemIsWaitingToDefrostAndCompressorOnTimeIsEqualToMinimumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMaximumOnInit)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldInitializeIntoWaitingForRemainingTimeBetweenDefrostsWhenSystemIsWaitingToDefrostAndCompressorOnTimeIsGreaterThanMinimumTimeBetweenDefrostsButLessThanMaxAndTimeBetweenDefrostsIsMaximumOnInit)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldInitializeIntoReadyAndDefrostingWhenSystemIsNotWaitingToDefrostOnInit)
{
   GivenWaitingToDefrostIs(false);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeAndDoorAccelerationsToZeroIfFreezerIsTooWarmAtPowerUpOnInit)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenFreezerTooWarmOnPowerUpIs(true);
   GivenReadyToDefrostIsInitialized();

   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldNotResetCompressorOnTimeAndDoorAccelerationsToZeroIfFreezerIsNotTooWarmAtPowerUpOnInit)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenFreezerTooWarmOnPowerUpIs(false);
   GivenReadyToDefrostIsInitialized();

   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
   DoorAccelerationsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldTransitionFromWaitingForMinimumTimeBetweenDefrostsToReadyAndDefrostingWhenNoLongerWaitingToDefrostAndShouldNotResetCompressorOnTimeAndDoorAccelerationsToZero)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
   DoorAccelerationsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldTransitionFromWaitingForRemainingTimeBetweenDefrostsToReadyAndDefrostingWhenNoLongerWaitingToDefrostAndShouldNotResetCompressorOnTimeAndDoorAccelerationsToZero)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   DoorAccelerationsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeHasNotReachedMinimumButSumOfCompressorOnTimeAndDoorAccelerationsIsEqualToMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeHasNotReachedMinimumButSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 2);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsEqualToMinimumTimeButSumOfCompressorOnTimeAndDoorAccelerationsIsLessThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsGreaterThanMinimumTimeButSumOfCompressorOnTimeAndDoorAccelerationsIsLessThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 2);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsLessThanMinimumTimeAndSumOfCompressorOnTimeAndDoorAccelerationsIsLessThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsLessThanMinimumTimeAndTimeBetweenDefrostsIsMin)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitIfCompressorOnTimeIsEqualToMinimumTimeAndTimeBetweenDefrostsIsMinAndTransitionToReadyAndDefrostingWhenWaitingToDefrostChangesToFalse)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitIfCompressorOnTimeIsGreaterThanMinimumTimeAndTimeBetweenDefrostsIsMin)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitWhenCompressorOnTimeIsEqualToMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsEqualToMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitWhenCompressorOnTimeIsEqualToMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitWhenCompressorOnTimeIsGreaterThanMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsEqualToMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostOnInitWhenCompressorOnTimeIsGreaterThanMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   After(0);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostOnInitWhenCompressorOnTimeIsLessThanMinimumTimeAndTheSumOfCompressorOnTimeAndDoorAccelerationsIsGreaterThanMaximumTimeBetweenDefrostsAndTimeBetweenDefrostsIsMax)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 2);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldTransitionFromReadyAndDefrostingToWaitingForMinimumTimeBetweenDefrostsWhenWaitingToDefrostChangesToTrueAndShouldResetCompressorOnTimeDoorAccelerationsAndWaitingForDefrostTimeInSecondsToZero)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   ReadyToDefrostShouldBe(false);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
   WaitingDefrostTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMaxTimeBetweenDefrostsWhenNoneOfTheConditionsCausingItToBeMinimumAreTrueOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmOnPowerUpIs(false);
   GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.maxTimeBetweenDefrostsInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMaxTimeBetweenDefrostsWhenNoneOfTheConditionsCausingItToBeMinimumAreTrueOnEntryOfWaitingForRemainingTimeBetweenDefrosts)
{
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsTrue();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.maxTimeBetweenDefrostsInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenFreshFoodDefrostWasAbnormalOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(true);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmOnPowerUpIs(false);
   GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenFreezerDefrostWasAbnormalOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(true);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmOnPowerUpIs(false);
   GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenConvertibleCompartmentDefrostWasAbnormalAndItHasAConvertibleCompartmentOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(true);
   GivenItHasAConvertibleCompartmentIs(true);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmOnPowerUpIs(false);
   GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldNotUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenConvertibleCompartmentDefrostWasAbnormalAndItDoesNotHaveAConvertibleCompartmentOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(true);
   GivenItHasAConvertibleCompartmentIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmOnPowerUpIs(false);
   GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.maxTimeBetweenDefrostsInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsOnEntryOfWaitingForMinimumTimeBetweenDefrostsBecauseInvalidFreezerEvaporatorThermistorChangedToTrueWhileDefrosting)
{
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenInvalidFreezerEvaporatorThermistorDuringDefrostChangesTo(true);
   WhenWaitingToDefrostIs(true);
   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenFreezerTooWarmAtPowerIsTrueOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmOnPowerUpIs(true);
   GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
   GivenEepromClearedFlagIs(false);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToMinTimeBetweenDefrostsWhenEepromClearedFlagIsTrueOnEntryOfWaitingForMinimumTimeBetweenDefrosts)
{
   GivenFreshFoodDefrostWasAbnormalIs(false);
   GivenFreezerDefrostWasAbnormalIs(false);
   GivenConvertibleCompartmentDefrostWasAbnormalIs(false);
   GivenInvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   GivenFreezerTooWarmOnPowerUpIs(false);
   GivenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(false);
   GivenEepromClearedFlagIs(true);

   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToAhamPrechillTimeWhenUseAhamPrechillReadyToDefrostIsSetToTrueWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(0);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes);
}

TEST(ReadyToDefrost, ShouldUpdateTimeBetweenDefrostsToAhamPrechillTimeWhenUseAhamPrechillReadyToDefrostIsSetToTrueWhileInWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterAhamPrechillTimeWhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueDuringWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);

   WhenCompressorTurns(ON);
   After(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterAhamPrechillTimeWhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueDuringWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);

   WhenCompressorTurns(ON);
   After(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeAndDoorAccelerationsWhenReceivingResetDefrostCountsSignalWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeAndDoorAccelerationsWhenReceivingResetDefrostCountsSignalWhileInWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeAndDoorAccelerationsWhenReceivingResetDefrostCountsSignalWhileInWaitingForReadyAndDefrostingState)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIsFalseAndReadyToDefrostIsInitialized();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterAhamPrechillTimeWhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueDuringReadyAndDefrosting)
{
   GivenTimeBetweenDefrostsIsMaximumAndWaitingToDefrostIsFalse();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenCompressorTurns(ON);

   After(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   TimeBetweenDefrostsInMinutesShouldBe(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingToDefrostTimeToTotalCompressorOnTimeOnEntryToWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateTotalCompressorOnTimeForTimeThatCompressorIsOnDuringWaitingForMinimumTimeBetweenDefrostsStateIfCompressorIsOnDuringEntryAndTurnsOffLater)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(MSEC_PER_SEC);
   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + 1);
}

TEST(ReadyToDefrost, ShouldUpdateTotalCompressorOnTimeForTimeThatCompressorIsOnDuringWaitingForMinimumTimeBetweenDefrostsStateIfCompressorTurnsOnAfterEntryThenTurnsOffLater)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(ON);

   After(MSEC_PER_SEC);
   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + 1);
}

TEST(ReadyToDefrost, ShouldAccumulateCompressorOnTimeAsItTurnsOnAndOffDuringWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   for(uint8_t i = 0; i < 10; i++)
   {
      WhenCompressorTurns(ON);

      After(MSEC_PER_SEC);
      WhenCompressorTurns(OFF);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + (i + 1));
   }
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdOnceCompressorOnTimeReachesMinimumTimeBetweenDefrostsWhenCompressorIsOnFromEntryOfWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(0);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN);
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdOnceCompressorOnTimeReachesMinimumTimeBetweenDefrostsWithCompressorTurningOnAndOffNotAtPeriodicUpdateTimeIntervalsDuringMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - (5 * UpdatePeriodInSeconds));
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);

   WhenCompressorTurns(ON);
   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);
   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);

   WhenCompressorTurns(ON);
   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);
   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2);

   WhenCompressorTurns(ON);
   After((4 * UpdatePeriodInSeconds + 1) * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdOnceCompressorOnTimeReachesMinimumTimeBetweenDefrostsWithCompressorTurningOnAndOffDuringMinimumTimeBetweenDefrostsStateAndStartingCompressorOnTimeGreaterThanZero)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 5 * UpdatePeriodInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   ReadyToDefrostShouldBe(false);

   After(4 * UpdatePeriodInSeconds * MSEC_PER_SEC);
   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1 * UpdatePeriodInSeconds);
   ReadyToDefrostShouldBe(false);

   WhenCompressorTurns(ON);
   After(1 * UpdatePeriodInSeconds * MSEC_PER_SEC);
   WhenCompressorTurns(OFF);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeToTotalCompressorTimeDuringWaitingForMinimumTimeBetweenDefrostsStatesWheneverCompressorTurnsOff)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   for(uint8_t i = 0; i < 10; i++)
   {
      WhenCompressorTurns(ON);

      After(SomeSeconds * MSEC_PER_SEC);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i * SomeSeconds);
      WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i * SomeSeconds);

      WhenCompressorTurns(OFF);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + (i + 1) * SomeSeconds);
   }
}

TEST(ReadyToDefrost, ShouldClearReadyToDefrostWhenStartingToWaitToDefrostTheNextTime)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(1 * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldStartOverAtZeroCompressorOnTimeWhenWaitingToDefrostChangesFromFalseToTrueAndThereWasAccumulatedCompressorOnTimeBeforeTheChange)
{
   GivenCompressorIs(ON);
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 1);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(1 * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   After(1);
   ReadyToDefrostShouldBe(false);
   CompressorOnTimeInSecondsShouldBe(0);

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostAfterMinimumTimeBetweenDefrostsIfCompressorWasNotOnForMinimumTimeBetweenDefrostsAndThenShouldSetReadyToDefrostOnceCompressorIsOnForThatLong)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(ON);

   After(SomeSeconds * MSEC_PER_SEC);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + SomeSeconds);

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(false);

   WhenCompressorTurns(ON);
   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - SomeSeconds * MSEC_PER_SEC - SomeCompressorOnTimeInSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeAfterPeriodicUpdateTimeEvenIfCompressorDidNotTurnOff)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeAfterPeriodicUpdateTimeEvenIfCompressorIsOff)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsAfterPeriodicUpdateTimeEvenIfCompressorDidNotTurnOff)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeEveryPeriodicUpdateTimeWhenCompressorDoesNotTurnOff)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i * UpdatePeriodInSeconds);

      After(1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + (i + 1) * UpdatePeriodInSeconds);
   }
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdWhenThereIsPeriodicUpdateTimeLeftToReachMinimumCompressorOnTime)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - UpdatePeriodInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeAfterPeriodicUpdateTimeAfterCompressorTurnedOn)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(OFF);

   WhenCompressorTurns(ON);
   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeEveryPeriodicUpdateTimeAfterCompressorTurnedOn)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(OFF);

   WhenCompressorTurns(ON);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + i * UpdatePeriodInSeconds);

      After(1);
      CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + (i + 1) * UpdatePeriodInSeconds);
   }
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostAfterCompressorTurnsOnAndRemainingMinimumTimeIsLessThanPeriodicUpdateTime)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - LessThanPeriodicUpdateInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenCompressorTurns(OFF);

   WhenCompressorTurns(ON);

   After(LessThanPeriodicUpdateInSeconds * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostAfterCompressorTurnsOnAndRemainingAhamPrechillTimeIsLessThanPeriodicUpdateTime)
{
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   WhenCompressorOnTimeInSecondsIs(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - LessThanPeriodicUpdateInSeconds);

   WhenCompressorTurns(ON);

   After(LessThanPeriodicUpdateInSeconds * MSEC_PER_SEC - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeAfterPeriodicUpdateTimeAfterCompressorTurnsOff)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);

   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);

   After(1);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds + UpdatePeriodInSeconds);
};

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenADoorClosesThatWasOpenOnInitWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   After(SomeSeconds * MSEC_PER_SEC);
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenMoreThanOneDoorIsAlreadyOpenOnInitAndThenOneClosesAfterTheOtherWhileWaitingForMinimumTimeBetweenDefrostsAndOnlyIncrementDoorAccelerationsForTimeThatADoorIsOpen)
{
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenFreezerDoorIs(Open);
   GivenConvertibleCompartmentDoorAsFreshFoodDoorIs(Open);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingToDefrostIsTrue();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenFreezerDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenConvertibleCompartmentAsFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(3 * SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenDoorClosesWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOnAndADoorIsAlreadyOpenOnInit)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeWhenDoorClosesWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsOffAndADoorIsAlreadyOpenOnInit)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeWhenDoorClosesWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOnAndADoorIsAlreadyOpenOnInit)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenDoorOpensWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOnAndDoorsAreClosedOnInit)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeWhenADoorOpensWithAnotherOneClosedWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOffAndAtLeastOneDoorIsOpenOnInit)
{
   GivenLeftSideFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeWhenDoorOpensWhileWaitingForMinimumTimeBetweenDefrostsIfCompressorIsAlreadyOnAndADoorIsClosedOnInit)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationAndCompressorOnTimeAfterDoorIsOpenThenClosed)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();

   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   CompressorOnTimeInSecondsShouldBe(2 * SomeSeconds);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostAfterCompressorIsOnForMinimumTimeWhileADoorOpenedThenClosedDuringMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 2 * SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostAfterCompressorIsOnForMinimumTimeWhileADoorClosedThenOpenedDuringMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 2 * SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationForOpenDoorWhenCompressorTurnsOffDuringMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldNotUpdateDoorAccelerationsWhenCompressorTurnsOffIfDoorIsClosedDuringMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationForOpenDoorThatClosesAfterCompressorTurnsOffDuringMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorThatOpensThenClosesWhileCompressorIsOffDuringMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(0);

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenCompressorTurnsOnAndAfterTheDoorClosesForAnOpenDoorDuringMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorThatIsStillOpenWhileAnotherDoorOpenThenClosesAndCompressorIsOffDuringMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostWhenTimerExpiresAfterInfiniteTimeFromWhenADoorClosedWhenCompressorWasNotOnForMinimumTime)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   After(UINT32_MAX);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenMinimumTimeReached)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(0);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   WhenFreezerDoorOpensAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(0);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(2 * SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 2 * SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe((defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE -
                                       2 * SomeSeconds) *
      defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe((defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE) * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeAndWaitingToDefrostTimeWhenMinimumTimeReachedAndCompressorIsOn)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);

   WhenCompressorTurnsOnAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - SomeSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeOrWaitingToDefrostTimeWhenTimerExpiresAfterInfiniteTimeAndCompressorIsNotOn)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeSeconds);

   After(UINT32_MAX);
   CompressorOnTimeInSecondsShouldBe(SomeSeconds);
   WaitingDefrostTimeInSecondsShouldBe(SomeSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsOnEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(SomeSeconds);
   GivenFreezerDoorAccelerationInSecondsIs(SomeSeconds);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WaitingDefrostTimeInSecondsShouldBe(2 * SomeSeconds + defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenCompressorTurnsOffAfterBeingOnUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   After(SomeSeconds * MSEC_PER_SEC);

   WhenCompressorTurns(OFF);
   CompressorOnTimeInSecondsShouldBe(SomeSeconds + defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsIsReachedWithCompressorOn)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) *
         MSEC_PER_MIN -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsIsReachedWithCompressorOnAndSomePreviousDoorAccelerations)
{
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(SomeSeconds);
   GivenFreezerDoorAccelerationInSecondsIs(SomeSeconds);
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) *
         MSEC_PER_MIN -
      (2 * SomeSeconds * MSEC_PER_SEC) - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsIsReachedWithCompressorOnAndADoorOpen)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   After(((defrostData.idleData.maxTimeBetweenDefrostsInMinutes -
             defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) *
            MSEC_PER_MIN) /
         (1 + defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForOpenDoorsWhenCompressorTurnsOffAfterBeingOnUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenCompressorTurnsOffAfterBeingOnUponEntryAndADoorWasAlsoOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds +
      SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
      SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsIsReachedWithCompressorOffAndADoorOpen)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   After(((defrostData.idleData.maxTimeBetweenDefrostsInMinutes - defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) * MSEC_PER_MIN) /
         (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForOpenDoorsWhenCompressorTurnsOnAfterBeingOffUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentAsFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenCompressorTurnsOnAfterBeingOffUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentAsFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
      SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorThatClosedWhenDoorClosesAfterBeingOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorsThatRemainOpenWhenADoorClosesAfterBeingOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenDoorClosesAndCompressorWasOnUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeWhenDoorClosesAndCompressorWasOffUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenDoorClosesAfterBeingOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds +
      SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
      SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsForDoorThatWasOpenWhenADifferentDoorOpensAfterBeingClosedUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(0);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenDoorOpensAndCompressorWasOnUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeWhenDoorOpensAndCompressorWasOffUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenDoorOpensAfterBeingOpenUponEntryToWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsClosedAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds +
      SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterTheCompressorTurnsOffButADoorRemainsOpen)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * MSEC_PER_SEC -
            SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterTheCompressorTurnsOffAndDoorsAreClosed)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds);

   After(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
      SomeSeconds * MSEC_PER_SEC -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterTheCompressorTurnsOnAndDoorsAreClosed)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();

   After(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterTheCompressorTurnsOnAndADoorIsOpen)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 + 1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorOpensAndTheCompressorIsOn)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * MSEC_PER_SEC) /
         (1 + 1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorOpensAndTheCompressorIsOff)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorOpensAfterSomeSeconds();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN) /
         (1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorClosesAndTheCompressorIsOn)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();

   After(
      defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
      SomeSeconds * MSEC_PER_SEC -
      SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorClosesAndTheCompressorIsOff)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorClosesAndTheCompressorIsOnAndAnotherDoorIsStillOpen)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * MSEC_PER_SEC -
            SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
            SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 + 1 * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterMaximumTimeBetweenDefrostsAfterADoorClosesAndTheCompressorIsOffAndAnotherDoorIsStillOpen)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOffAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
            SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdWhenUseAhamPrechillReadyToDefrostIsSetToTrueWhileInWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenMaximumTimeReachedForOpenDoors)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOffAndTimeBetweenDefrostsIsMaximumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
            SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   LeftSideFreshFoodDoorAccelerationsShouldBe(SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   FreezerDoorAccelerationsShouldBe((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
                                       defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE -
                                       SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) /
      (1 * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond) * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenMaximumTimeReachedIfCompressorIsOn)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorClosesAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);

   After(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
      defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
      SomeSeconds * MSEC_PER_SEC -
      SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   CompressorOnTimeInSecondsShouldBe((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
      SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond));
}

TEST(ReadyToDefrost, ShouldUpdateCompressorOnTimeWhenMaximumTimeReachedIfCompressorIsTurnedOnLater)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();
   CompressorOnTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 + 1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   CompressorOnTimeInSecondsShouldBe((defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
                                        defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
                                        SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) /
         (1 + 1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) +
      SomeSeconds);
}

TEST(ReadyToDefrost, ShouldUpdateWaitingDefrostTimeInSecondsWhenMaximumTimeReached)
{
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOnAfterSomeSeconds();

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN -
            defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN -
            SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * MSEC_PER_SEC) /
         (1 + 1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
   WaitingDefrostTimeInSecondsShouldBe((defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
                                          defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
                                          SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) /
         (1 + 1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) +
      defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond *
         (((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
             defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE)) /
            (1 + 1 * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond)));
}

TEST(ReadyToDefrost, ShouldNotResetWaitingDefrostTimeInSecondsToZeroWhenNoLongerWaitingToDefrost)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenCompressorTurnsOffAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);

   WhenWaitingToDefrostIs(false);
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);
}

TEST(ReadyToDefrost, ShouldTransitionFromWaitingForMinimumTimeBetweenDefrostsToWaitingForRemainingTimeBetweenDefrostsWhenTimerExpiresAndItIsNotReadyToDefrost)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsBelowMinimumByOneSecondAndWaitingToDefrostIsTrue();

   After(1 * MSEC_PER_SEC - 1);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   After(1);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForRemainingTimeBetweenDefrosts);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldNotSetReadyToDefrostErdAfterMinimumTimeWhenResetDefrostCountsSignalWasReceivedWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(0);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);

   After(1);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldRestartAhamPrechillTimeWhenResetDefrostCountsSignalWasReceivedAndSetReadyToDefrostErdAfterCompressorHasBeenOnForMinimumTimeWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIsOnForMinimumTimeAndUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErdChangedToTrueOneMillisecondBeforeTheMinimumTime();

   After(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsPeriodicallyEvenIfDoorsDoNotCloseWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsClosedAndConvertibleCompartmentDoorAsFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      LeftSideFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationsShouldBe(0);
      ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      LeftSideFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationsShouldBe(0);
      ConvertibleCompartmentDoorAsFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsPeriodicallyAfterADoorOpensAndCompressorIsOffWhileWaitingForMinimumTimeBetweenDefrostsAndStopUpdatingAfterTheDoorCloses)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenLeftSideFreshFoodDoorIs(Open);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      LeftSideFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      LeftSideFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldStopUpdatingDoorAccelerationsPeriodicallyAfterTheDoorClosesWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenLeftSideFreshFoodDoorHasBeenOpenForTenPeriodicUpdatePeriodsAndCompressorIsOff();

   WhenLeftSideFreshFoodDoorIs(Closed);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      LeftSideFreshFoodDoorAccelerationsShouldBe(10 * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);

      After(1);
      LeftSideFreshFoodDoorAccelerationsShouldBe(10 * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeOrSetReadyToDefrostErdAfterCompressorTurnsOffAndWasNotOnLongEnoughToReachMinimumTimeWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorIsOnForOneUpdatePeriod();
   CompressorOnTimeInSecondsShouldBe(UpdatePeriodInSeconds);

   WhenCompressorIsOffForOneUpdatePeriod();
   CompressorOnTimeInSecondsShouldBe(UpdatePeriodInSeconds);

   After(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * MSEC_PER_MIN - 2 * UpdatePeriodInSeconds * MSEC_PER_SEC);
   CompressorOnTimeInSecondsShouldBe(UpdatePeriodInSeconds);
   ReadyToDefrostShouldBe(false);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterALessThanPeriodicUpdateTimeAfterADoorOpensWhenCompressorHasAccumulatedMinimumTimeBetweenDefrostsWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 2 * (UpdatePeriodInSeconds));
   GivenLeftSideFreshFoodDoorIsClosedAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   After((3 * UpdatePeriodInSeconds * MSEC_PER_SEC) / 2);
   WhenLeftSideFreshFoodDoorIs(Opened);

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2) - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsWhenSomeDoorOpenTimeAfterResetDefrostCountsRequestReceivedWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(0);
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(SomeDoorAccelerationsInSecondsPerSecond);
   GivenTimeBetweenDefrostsIsMinimumAndWaitingForDefrostIsTrue();

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   LeftSideFreshFoodDoorAccelerationsShouldBe(0);
   After(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN + SomeSeconds * MSEC_PER_SEC);

   WhenLeftSideFreshFoodDoorIs(Closed);
   LeftSideFreshFoodDoorAccelerationsShouldBe((defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE + SomeSeconds) *
      defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldResetWaitingForDefrostTimeInSecondsWhenResetDefrostCountRequestReceivedWhileInWaitingForMinimumTimeBetweenDefrostsState)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenWaitingToDefrostIsTrueAndReadyToDefrostIsInitialized();

   WhenCompressorTurns(ON);

   After(SomeSeconds * MSEC_PER_SEC);
   WaitingDefrostTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   WaitingDefrostTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldResetWaitingForDefrostTimeInSecondsWhenResetDefrostCountRequestReceivedWhileInWaitingForRemainingTimeBetweenDefrostsState)
{
   GivenLeftSideFreshFoodDoorIsOpenAndFreezerDoorIsOpenAndCompressorIsOnAndWaitingForRemainingTimeBetweenDefrosts();

   WhenCompressorTurnsOffAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE +
      SomeSeconds +
      SomeSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
      SomeSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   WaitingDefrostTimeInSecondsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterAhamPrechillTimeWhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErdChangesToTrueDuringReadyAndDefrostingAfterWaitingToDefrostIsTrue)
{
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsFalseAndStateIsReadyAndDefrosting();

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   WhenWaitingToDefrostIs(true);

   WhenCompressorTurns(ON);
   After(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUseMaximumTimeOnNextDefrostAfterBeingForcedToUseAhamPrechillTimeByUseAhamPrechillErd)
{
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsEqualToMinimumAndWaitingToDefrostIsFalseAndStateIsReadyAndDefrosting();

   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   WhenWaitingToDefrostIs(true);

   WhenCompressorTurns(ON);
   After(defrostData.idleData.ahamPrechillTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);

   WhenDefrostChangesFromWaitingToDefrostToDefrostingBackToWaitingToDefrost();

   After(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_MIN - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}
TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterALessThanPeriodicUpdateTimeAfterADoorClosesWhenCompressorHasAccumulatedMinimumTimeBetweenDefrostsWhileWaitingForMinimumTimeBetweenDefrosts)
{
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE - 2 * (UpdatePeriodInSeconds));
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOnAndItIsWaitingForMinimumTimeBetweenDefrosts();

   After((3 * UpdatePeriodInSeconds * MSEC_PER_SEC) / 2);
   WhenLeftSideFreshFoodDoorIs(Closed);

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2) - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsCorrectlyAfterUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErdChangesToTrueDuringAPeriodicUpdatePeriod)
{
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(SomeDoorAccelerationsInSecondsPerSecond);
   GivenLeftSideFreshFoodDoorIsOpenAndCompressorIsOffAndItIsWaitingForMinimumTimeBetweenDefrosts();

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2));
   WhenUseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsIs(true);
   LeftSideFreshFoodDoorAccelerationsShouldBe(0);

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2));
   LeftSideFreshFoodDoorAccelerationsShouldBe(0);

   After((UpdatePeriodInSeconds * MSEC_PER_SEC / 2) - 1);
   LeftSideFreshFoodDoorAccelerationsShouldBe(0);

   After(1);
   LeftSideFreshFoodDoorAccelerationsShouldBe(((UpdatePeriodInSeconds * MSEC_PER_SEC) / MSEC_PER_SEC) * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterHalfAPeriodicUpdatePeriodWithTheCompressorOnIfCompressorOnTimeIsMaxMinusHalfAPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (UpdatePeriodInSeconds) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2 - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterHalfAPeriodicUpdatePeriodWithADoorOpenAndCompressorOffIfCompressorOnTimeIsMaxMinusHalfAPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds)) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2 - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterHalfAPeriodicUpdatePeriodFromWhenTheCompressorTurnedOnIfCompressorOnTimeIsMaxMinusHalfAPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (UpdatePeriodInSeconds) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenCompressorTurns(ON);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2 - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterHalfAPeriodicUpdatePeriodFromWhenCompressorTurnsOffIfThereIsADoorOpenAndIfCompressorOnTimeIsMaxMinusHalfAPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds)) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenCompressorTurns(OFF);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC / 2 - 1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldNotUpdateCompressorOnTimeAfterCompressorHasBeenTurnedOffWhileWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenLeftSideFreshFoodDoorIs(Closed);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - 10 * (UpdatePeriodInSeconds));
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenCompressorTurns(OFF);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC);
      CompressorOnTimeInSecondsShouldBe(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - 10 * (UpdatePeriodInSeconds));
   }
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterATimeLessThanPeriodicUpdatePeriodFromWhenADoorOpensIfCompressorIsOnAndIfRemainingTimeUntilReadyIsLessThanPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenLeftSideFreshFoodDoorIs(Closed);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds)) / 2);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftSideFreshFoodDoorIs(Opened);

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
            (defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
               (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds * MSEC_PER_SEC)) / 2)) /
         (1 + defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterATimeLessThanPeriodicUpdatePeriodFromWhenADoorOpensIfCompressorIsOffAndIfRemainingTimeUntilReadyIsLessThanPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftSideFreshFoodDoorIs(Closed);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds / 2)));
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftSideFreshFoodDoorIs(Opened);

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
            (defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
               (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds * MSEC_PER_SEC)) / 2)) /
         (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond) -
      1);
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterATimeLessThanPeriodicUpdatePeriodFromWhenADoorClosesIfCompressorIsOnAndIfRemainingTimeUntilReadyIsLessThanPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(ON);
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (UpdatePeriodInSeconds / 2));
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftSideFreshFoodDoorIs(Closed);

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
      (defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
         (UpdatePeriodInSeconds * MSEC_PER_SEC / 2)) -
      1));
   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldSetReadyToDefrostErdAfterATimeLessThanPeriodicUpdatePeriodFromWhenADoorClosesIfCompressorIsOffAndADoorIsStillOpenAndIfRemainingTimeUntilReadyIsLessThanPeriodicUpdatePeriodWhenInitializedIntoWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenFreezerDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE - (defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds / 2)));
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftSideFreshFoodDoorIs(Closed);

   After((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
            (defrostData.idleData.maxTimeBetweenDefrostsInMinutes * MSEC_PER_SEC -
               (defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond * (UpdatePeriodInSeconds * MSEC_PER_SEC)) / 2)) /
         (defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond) -
      1);

   ReadyToDefrostShouldBe(false);

   After(1);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldUpdateDoorAccelerationsPeriodicallyWhileWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenFreezerDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   uint8_t maxPeriodicUpdatesBeforeReadyToDefrost = ((defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
                                                        defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE) /
                                                       (defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond +
                                                          defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond)) /
      UpdatePeriodInSeconds;

   for(uint8_t i = 0; i < maxPeriodicUpdatesBeforeReadyToDefrost; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC - 1);
      LeftSideFreshFoodDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationsShouldBe(i * UpdatePeriodInSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);

      After(1);
      LeftSideFreshFoodDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData.idleData.freshFoodDoorIncrementFactorInSecondsPerSecond);
      FreezerDoorAccelerationsShouldBe((i + 1) * UpdatePeriodInSeconds * defrostData.idleData.freezerDoorIncrementFactorInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldNotUpdateDoorAccelerationsAfterAllDoorsHaveBeenClosedWhileWaitingForRemainingTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenLeftSideFreshFoodDoorIs(Open);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftSideFreshFoodDoorIs(Closed);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(UpdatePeriodInSeconds * MSEC_PER_SEC);
      LeftSideFreshFoodDoorAccelerationsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);
   }
}

TEST(ReadyToDefrost, ShouldBeReadyToDefrostIfCompressorOnTimeAccumulatedGreaterThanNewMaximumTimeBetweenDefrostsAfterAReset)
{
   GivenCompressorIs(OFF);
   GivenLeftSideFreshFoodDoorIs(Closed);
   GivenFreezerDoorIs(Closed);
   GivenWaitingToDefrostIs(true);
   GivenCompressorOnTimeInSecondsIs(shorterTimeBetweenDefrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenLeftSideFreshFoodDoorAccelerationInSecondsIs((shorterTimeBetweenDefrostData.idleData.maxTimeBetweenDefrostsInMinutes - shorterTimeBetweenDefrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes) * SECONDS_PER_MINUTE - 1);
   GivenFreezerDoorAccelerationInSecondsIs(1);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitializedWithLesserTimeBetweenDefrostsInParametric();

   WaitingDefrostTimeInSecondsShouldBe(shorterTimeBetweenDefrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE);
   TimeBetweenDefrostsInMinutesShouldBe(shorterTimeBetweenDefrostData.idleData.maxTimeBetweenDefrostsInMinutes);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldBeReadyToDefrostWhenCompressorOnTimeIsPurposefullyIncrementedToMinTimeBetweenDefrosts)
{
   GivenCompressorOnTimeIsZeroAndTimeBetweenDefrostsIsMinimumAndSystemIsInWaitingForMinimumTimeBetweenDefrosts();

   WhenCompressorTurns(ON);
   WhenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldBeReadyToDefrostWhenCompressorOnTimeIsPurposefullyIncrementedToMaxTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenCompressorTurns(ON);
   WhenCompressorOnTimeInSecondsIs(defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldBeReadyToDefrostWhenDoorAccelerationIsPurposefullyIncrementedSoThatCompressorOnTimePlusDoorAccelerationTimeIsEqualToMaxTimeBetweenDefrosts)
{
   GivenCompressorIs(OFF);
   GivenCompressorOnTimeInSecondsIs(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE);
   GivenWaitingToDefrostIs(true);
   GivenSystemConditionsAreSuchThatTimeBetweenDefrostsIsMaximum();
   GivenReadyToDefrostIsInitialized();

   WhenLeftSideFreshFoodDoorIs(Open);
   WhenLeftSideFreshFoodDoorAccelerationInSecondsIs(
      defrostData.idleData.maxTimeBetweenDefrostsInMinutes * SECONDS_PER_MINUTE -
      defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + 1);

   After(UpdatePeriodInSeconds * MSEC_PER_SEC);
   ReadyToDefrostShouldBe(true);
}

TEST(ReadyToDefrost, ShouldResetCompressorOnTimeInSecondsAndDoorAccelerationsToZeroWhenWaitingToDefrostChangesFromFalseToTrue)
{
   GivenCompressorOnTimeInSecondsIs(SomeCompressorOnTimeInSeconds);
   GivenDoorAccelerationsAre(SomeDoorAccelerationsInSecondsPerSecond);
   GivenWaitingToDefrostIs(true);
   GivenReadyToDefrostIsInitialized();

   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   WhenWaitingToDefrostIs(false);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_ReadyAndDefrosting);
   CompressorOnTimeInSecondsShouldBe(SomeCompressorOnTimeInSeconds);
   DoorAccelerationsShouldBe(SomeDoorAccelerationsInSecondsPerSecond);

   WhenWaitingToDefrostIs(true);
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);
   CompressorOnTimeInSecondsShouldBe(0);
   DoorAccelerationsShouldBe(0);
}

TEST(ReadyToDefrost, ShouldResetWaitingDefrostTimeInSecondsToZeroWhenWaitingToDefrostChangesFromFalseToTrue)
{
   GivenCompressorIs(ON);
   GivenTimeBetweenDefrostsIsMaximumAndCompressorOnTimeIsAtMinimumAndInWaitingForRemainingTimeBetweenDefrostsState();

   WhenCompressorTurnsOffAfterSomeSeconds();
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);

   WhenWaitingToDefrostIs(false);
   WaitingDefrostTimeInSecondsShouldBe(defrostData.idleData.minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes * SECONDS_PER_MINUTE + SomeSeconds);

   WhenWaitingToDefrostIs(true);
   WaitingDefrostTimeInSecondsShouldBe(0);
}
