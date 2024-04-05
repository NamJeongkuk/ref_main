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
#include "ConvertibleCompartmentStateType.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When

enum
{
   Invalid = false,
   Valid = true,
};

static const DefrostHeaterMaxOnTimeConfiguration_t config = {
   .freezerEvaporatorThermistorIsValidResolvedErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodEvaporatorThermistorIsValidResolvedErd = Erd_FreshFoodEvapThermistor_IsValidResolved,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentEvaporatorThermistorIsValidResolvedErd = Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

static const DefrostHeaterData_t freshFoodHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 50,
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 15,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 35,
   .defrostTerminationTemperatureInDegFx100 = 4460
};

static const DefrostHeaterData_t freezerHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 60,
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .defrostTerminationTemperatureInDegFx100 = 5900
};

static DefrostConvertibleCompartmentCommonHeaterData_t convertibleCompartmentHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 30,
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

TEST_GROUP(DefrostHeaterMaxOnTime)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   DefrostHeaterMaxOnTime_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void DefrostHeaterMaxOnTimeIsInitialized()
   {
      DefrostHeaterMaxOnTime_Init(&instance, dataModel, &config, &heaterOnData);
   }

   void FreezerEvaporatorThermistorValidityChangesTo(bool valid)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &valid);
   }

   void FreshFoodEvaporatorThermistorValidityChangesTo(bool valid)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvapThermistor_IsValidResolved, &valid);
   }

   void ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(bool valid)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved, &valid);
   }

   void ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t type)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &type);
   }

   void ThereIsAConvertibleCompartment()
   {
      DataModel_Write(dataModel, Erd_HasConvertibleCompartment, set);
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

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreezerMaxOnTimeErdToInvalidMaxOnTimeWhenFreezerEvaporatorThermistorIsInvalidOnInit)
{
   Given FreezerEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.freezerHeater.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateFreezerMaxOnTimeErdWhenValidityChanges)
{
   Given FreezerEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.freezerHeater.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When FreezerEvaporatorThermistorValidityChangesTo(Valid);
   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreshFoodMaxOnTimeErdToMaxOnTimeWhenFreshFoodEvaporatorThermistorIsValidOnInit)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Valid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreshFoodMaxOnTimeErdToInvalidMaxOnTimeWhenFreshFoodEvaporatorThermistorIsInvalidOnInit)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.freshFoodHeater.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateFreshFoodMaxOnTimeErdWhenValidityChanges)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.freshFoodHeater.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When FreshFoodEvaporatorThermistorValidityChangesTo(Valid);
   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToConvertibleCompartmentAsMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsValidAndConvertibleCompartmentIsActingAsFreshFoodOnInit)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Valid);
   Given ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToConvertibleCompartmentAsMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsValidAndConvertibleCompartmentIsActingAsFreezerOnInit)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Valid);
   Given ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToInvalidFreshFoodMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsInvalidAndConvertibleCompartmentIsActingAsFreshFoodOnInit)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.convertibleCompartmentHeaterAsFreshFood.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToInvalidFreezerMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsInvalidAndConvertibleCompartmentIsActingAsFreezerOnInit)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.convertibleCompartmentHeaterAsFreezer.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateConvertibleCompartmentMaxOnTimeErdWhenValidityChanges)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.convertibleCompartmentHeaterAsFreezer.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Valid);
   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateConvertibleCompartmentMaxOnTimeErdWhenConvertibleCompartmentStateChanges)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.convertibleCompartmentHeaterAsFreezer.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   Given ConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(heaterOnData.convertibleCompartmentHeaterAsFreshFood.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}
