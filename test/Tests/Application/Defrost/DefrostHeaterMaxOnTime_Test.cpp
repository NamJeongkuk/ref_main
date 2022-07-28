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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "DefrostData_TestDouble.h"

#define Given
#define When

enum
{
   Invalid = false,
   Valid = true,
};

static const DefrostHeaterMaxOnTimeConfiguration_t config = {
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .freshFoodEvaporatorThermistorIsValidErd = Erd_FreshFoodEvaporatorThermistorIsValid,
   .convertibleCompartmentEvaporatorThermistorIsValidErd = Erd_ConvertibleCompartmentEvaporatorThermistorIsValid,
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
   PersonalityParametricData_t personalityParametricData;
   DefrostHeaterMaxOnTime_t instance;
   DefrostData_t defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      DefrostData_TestDouble_Init(&defrostData);

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

   void ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_t type)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &type);
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
   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_FreshFood);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldSetConvertibleCompartmentMaxOnTimeErdToInvalidFreezerMaxOnTimeWhenConvertibleCompartmentEvaporatorThermistorIsInvalidAndConvertibleCompartmentIsActingAsFreshFoodOnInit)
{
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateConvertibleCompartmentMaxOnTimeErdWhenValidityChanges)
{
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   When ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Valid);
   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes);
}

TEST(DefrostHeaterMaxOnTime, ShouldUpdateConvertibleCompartmentMaxOnTimeErdWhenConvertibleCompartmentStateChanges)
{
   Given ConvertibleCompartmentEvaporatorThermistorValidityChangesTo(Invalid);
   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_Freezer);
   DefrostHeaterMaxOnTimeIsInitialized();

   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);

   Given ConvertibleCompartmentStateTypeChangesTo(ConvertibleCompartmentStateType_FreshFood);
   TheConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutesShouldBe(defrostData.freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
}
