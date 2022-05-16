/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostHeaterMaxOnTime.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "ConvertibleCompartmentCabinetStateType.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"

#define Given
#define When

enum
{
   Invalid = false,
   Valid = true,
};

static const DefrostData_t defrostData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .freezerAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .dmFreezerDefrostTemperatureInDegFx100 = 1500,
   .prechillFreezerSetpointInDegFx100 = 600,
   .prechillFreshFoodSetpointInDegFx100 = 4600,
   .prechillConvertibleCompartmentSetpointInDegFx100 = 600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes = 10,
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
   .dsmFreezerSetpointTemperatureInDegFx100 = 200,
   .defrostPeriodicTimeoutInSeconds = 1,
   .threeWayValvePositionToExitIdle = ValvePosition_B,
   .threeWayValvePositionForMaxPrechillHoldoff = ValvePosition_B,
   .threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost = ValvePosition_B
};

static const DefrostHeaterMaxOnTimeConfiguration_t config = {
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .freshFoodEvaporatorThermistorIsValidErd = Erd_FreshFoodEvaporatorThermistorIsValid,
   .convertibleCompartmentEvaporatorThermistorIsValidErd = Erd_ConvertibleCompartmentEvaporatorThermistorIsValid,
   .convertibleCompartmentCabinetStateErd = Erd_ConvertibleCompartmentCabinetState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

TEST_GROUP(DefrostHeaterMaxOnTime)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DefrostHeaterMaxOnTime_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void DefrostHeaterMaxOnTimeIsInitialized()
   {
      DefrostHeaterMaxOnTime_Init(&instance, dataModel, &config);
   }

   void FreezerEvaporatorThermistorValidityChangesTo(bool valid)
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorIsValid, &valid);
   }

   void FreshFoodEvaporatorThermistorValidityChangesTo(bool valid)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvaporatorThermistorIsValid, &valid);
   }

   void ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(bool valid)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvaporatorThermistorIsValid, &valid);
   }

   void ConvertibleCompartmentCabinetStateChangesTo(ConvertibleCompartmentCabinetStateType_t type)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinetState, &type);
   }

   void TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeaterMaxOnTimeInMinutes, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(DefrostHeaterMaxOnTime, ShouldSetFreezerMaxOnTimeErdToMaxOnTimeWhenFreezerEvaporatorThermistorIsValidOnInit)
{
   Given FreezerEvaporatorThermistorValidityChangesTo(Valid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreezerMaxOnTimeErdToInvalidMaxOnTimeWhenFreezerEvaporatorThermistorIsInvalidOnInit)
{
   Given FreezerEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateFreezerMaxOnTimeErdWhenValidityChanges)
{
   Given FreezerEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When FreezerEvaporatorThermistorValidityChangesTo(Valid);
   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreshFoodMaxOnTimeErdToMaxOnTimeWhenFreshFoodEvaporatorThermistorIsValidOnInit)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Valid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freshFoodDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreshFoodMaxOnTimeErdToInvalidMaxOnTimeWhenFreshFoodEvaporatorThermistorIsInvalidOnInit)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateFreshFoodMaxOnTimeErdWhenValidityChanges)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When FreshFoodEvaporatorThermistorValidityChangesTo(Valid);
   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freshFoodDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsValidOnInit)
{
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Valid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToInvalidFreshFoodMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsInvalidAndConvertibleCompartmentIsActingAsFreshFoodOnInit)
{
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentCabinetStateChangesTo(ConvertibleCompartmentCabinetState_FreshFood);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToInvalidFreezerMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsInvalidAndConvertibleCompartmentIsActingAsFreshFoodOnInit)
{
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentCabinetStateChangesTo(ConvertibleCompartmentCabinetState_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateConvertibleCompartmentMaxOnTimeErdWhenValidityChanges)
{
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentCabinetStateChangesTo(ConvertibleCompartmentCabinetState_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Valid);
   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateConvertibleCompartmentMaxOnTimeErdWhenConvertibleCabinetStateChanges)
{
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentCabinetStateChangesTo(ConvertibleCompartmentCabinetState_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   Given ConvertibleCompartmentCabinetStateChangesTo(ConvertibleCompartmentCabinetState_FreshFood);
   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}
