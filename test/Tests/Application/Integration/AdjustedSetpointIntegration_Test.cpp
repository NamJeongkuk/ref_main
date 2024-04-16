/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"

#define When
#define And

enum
{
   SomeSetpoint = 5000,
   SomeShift = 4,
};

TEST_GROUP(AdjustedSetpointIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;

   void setup()
   {
   }

   void GivenTheDataModelIsInitializedWith(PersonalityId_t personality)
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, personality);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenTheApplicationHasBeenInitialized()
   {
      Application_Init(&instance, dataModel, resetReason);
   }

   void GivenApplicationHasBeenInitializedWithPersonality(PersonalityId_t personality)
   {
      GivenTheDataModelIsInitializedWith(personality);
      GivenTheApplicationHasBeenInitialized();
   }

   void TheConvertibleCompartmentResolvedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartment_ResolvedSetpointInDegFx100, &setpoint);
   }

   void TheConvertibleCompartmentThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartment_ThermalShiftInDegFx100, &shift);
   }

   void TheConvertibleCompartmentAdjustedSetpointShouldBeCorrectlyShifted()
   {
      TemperatureDegFx100_t setpoint;
      DataModel_Read(dataModel, Erd_ConvertibleCompartment_AdjustedSetpointWithoutShiftInDegFx100, &setpoint);

      TemperatureDegFx100_t shift;
      DataModel_Read(dataModel, Erd_ConvertibleCompartment_ThermalShiftInDegFx100, &shift);

      TemperatureDegFx100_t adjustedSetpoint;
      DataModel_Read(dataModel, Erd_ConvertibleCompartment_AdjustedSetpointInDegFx100, &adjustedSetpoint);

      CHECK_EQUAL(setpoint + shift, adjustedSetpoint);
   }

   void TheConvertibleCompartmentAdjustedSetpointShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartment_AdjustedSetpointInDegFx100, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheFreezerResolvedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel, Erd_Freezer_ResolvedSetpointInDegFx100, &setpoint);
   }

   void TheFreezerThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel, Erd_Freezer_ThermalShiftInDegFx100, &shift);
   }

   void TheFreezerAdjustedSetpointShouldBeCorrectlyShifted()
   {
      TemperatureDegFx100_t setpoint;
      DataModel_Read(dataModel, Erd_Freezer_AdjustedSetpointWithoutShiftInDegFx100, &setpoint);

      TemperatureDegFx100_t shift;
      DataModel_Read(dataModel, Erd_Freezer_ThermalShiftInDegFx100, &shift);

      TemperatureDegFx100_t adjustedSetpoint;
      DataModel_Read(dataModel, Erd_Freezer_AdjustedSetpointInDegFx100, &adjustedSetpoint);

      CHECK_EQUAL(setpoint + shift, adjustedSetpoint);
   }

   void TheFreshFoodResolvedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreshFood_ResolvedSetpointInDegFx100, &setpoint);
   }

   void TheFreshFoodThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel, Erd_FreshFood_ThermalShiftInDegFx100, &shift);
   }

   void TheFreshFoodAdjustedSetpointShouldBeCorrectlyShifted()
   {
      TemperatureDegFx100_t setpoint;
      DataModel_Read(dataModel, Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100, &setpoint);

      TemperatureDegFx100_t shift;
      DataModel_Read(dataModel, Erd_FreshFood_ThermalShiftInDegFx100, &shift);

      TemperatureDegFx100_t adjustedSetpoint;
      DataModel_Read(dataModel, Erd_FreshFood_AdjustedSetpointInDegFx100, &adjustedSetpoint);

      CHECK_EQUAL(setpoint + shift, adjustedSetpoint);
   }
};

TEST(AdjustedSetpointIntegration, ShouldCalculateTheAdjustedSetpointForFourDoorPersonality)
{
   GivenApplicationHasBeenInitializedWithPersonality(TddPersonality_DevelopmentDualEvapFourDoor);
   TheConvertibleCompartmentAdjustedSetpointShouldBeCorrectlyShifted();
   TheFreezerAdjustedSetpointShouldBeCorrectlyShifted();
   TheFreshFoodAdjustedSetpointShouldBeCorrectlyShifted();

   When TheConvertibleCompartmentResolvedSetpointIs(SomeSetpoint);
   And TheConvertibleCompartmentThermalShiftIs(SomeShift);
   TheConvertibleCompartmentAdjustedSetpointShouldBeCorrectlyShifted();

   When TheFreezerResolvedSetpointIs(SomeSetpoint);
   And TheFreezerThermalShiftIs(SomeShift);
   TheFreezerAdjustedSetpointShouldBeCorrectlyShifted();

   When TheFreshFoodResolvedSetpointIs(SomeSetpoint);
   And TheFreshFoodThermalShiftIs(SomeShift);
   TheFreshFoodAdjustedSetpointShouldBeCorrectlyShifted();
}

TEST(AdjustedSetpointIntegration, ShouldCalculateTheAdjustedSetpointForFreezer)
{
   GivenApplicationHasBeenInitializedWithPersonality(TddPersonality_DevelopmentSingleDoorSingleEvapAllFreezer);

   When TheFreezerResolvedSetpointIs(SomeSetpoint);
   And TheFreezerThermalShiftIs(SomeShift);
   TheFreezerAdjustedSetpointShouldBeCorrectlyShifted();
}

TEST(AdjustedSetpointIntegration, ShouldCalculateTheAdjustedSetpointForFreshFood)
{
   GivenApplicationHasBeenInitializedWithPersonality(TddPersonality_DevelopmentSingleDoorSingleEvapAllFreshFood);

   When TheFreshFoodResolvedSetpointIs(SomeSetpoint);
   And TheFreshFoodThermalShiftIs(SomeShift);
   TheFreshFoodAdjustedSetpointShouldBeCorrectlyShifted();
}
