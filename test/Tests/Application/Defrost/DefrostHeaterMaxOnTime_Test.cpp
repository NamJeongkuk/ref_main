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

TEST_GROUP(DefrostHeaterMaxOnTime)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   DefrostHeaterMaxOnTime_t instance;
   const DefrostData_t *defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   }

   void DefrostHeaterMaxOnTimeIsInitialized()
   {
      DefrostHeaterMaxOnTime_Init(&instance, dataModel, &config);
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

   void ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_t type)
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

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freezerDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreezerMaxOnTimeErdToInvalidMaxOnTimeWhenFreezerEvaporatorThermistorIsInvalidOnInit)
{
   Given FreezerEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateFreezerMaxOnTimeErdWhenValidityChanges)
{
   Given FreezerEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When FreezerEvaporatorThermistorValidityChangesTo(Valid);
   TheFreezerDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freezerDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreshFoodMaxOnTimeErdToMaxOnTimeWhenFreshFoodEvaporatorThermistorIsValidOnInit)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Valid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freshFoodDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetFreshFoodMaxOnTimeErdToInvalidMaxOnTimeWhenFreshFoodEvaporatorThermistorIsInvalidOnInit)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateFreshFoodMaxOnTimeErdWhenValidityChanges)
{
   Given FreshFoodEvaporatorThermistorValidityChangesTo(Invalid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When FreshFoodEvaporatorThermistorValidityChangesTo(Valid);
   TheFreshFoodDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freshFoodDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsValidOnInit)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Valid);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToInvalidFreshFoodMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsInvalidAndConvertibleCompartmentIsActingAsFreshFoodOnInit)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_FreshFood);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToInvalidFreezerMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsInvalidAndConvertibleCompartmentIsActingAsFreshFoodOnInit)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateConvertibleCompartmentMaxOnTimeErdWhenValidityChanges)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Valid);
   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateConvertibleCompartmentMaxOnTimeErdWhenConvertibleCompartmentStateChanges)
{
   Given ThereIsAConvertibleCompartment();
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_FreshFood);
   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData->heaterOnData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}
