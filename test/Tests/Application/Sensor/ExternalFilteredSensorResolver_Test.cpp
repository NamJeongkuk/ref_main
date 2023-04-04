/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ExternalFilteredSensorResolver.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Invalid = false,
   Valid = true,
   InternalTemperatureInDegFx100 = 400,
   SomeInvalidInternalTemperatureInDegFx100 = InternalTemperatureInDegFx100 - 1,
   ExternalTemperatureInDegFx100 = 200,
   SomeInvalidExternalTemperatureInDegFx100 = ExternalTemperatureInDegFx100 - 1,
};

static const ExternalFilteredSensorResolverConfig_t config = {
   .internalFilteredSensorErd = Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,
   .internalFilteredSensorIsValidErd = Erd_AmbientThermistor_IsValidResolved,
   .externalFilteredSensorErd = Erd_Ambient_ExternalTemperatureInDegFx100,
   .externalFilteredSensorIsValidErd = Erd_Ambient_ExternalThermistorIsValid,
   .resolvedFilteredSensorErd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
   .resolvedFilteredSensorIsValidErd = Erd_Ambient_ResolvedThermistorIsValid,
   .sensorType = ExternalFilteredSensorResolver_SensorType_Temperature,
};

static const SensorDataSensorType_t ambientSensorDataWithCheck = {
   .lookupTable = NULL,
   .fallbackValueDegFx100 = 0,
   .alphaNum = 0,
   .alphaDenom = 0,
   .windowSize = 0,
   .clampData = { 0, 0 },
   .goodReadingCounterMax = 0,
   .badReadingCounterMax = 0,
   .enableExternalSensorCheck = true,
};

static const SensorDataSensorType_t ambientSensorDataWithoutCheck = {
   .lookupTable = NULL,
   .fallbackValueDegFx100 = 0,
   .alphaNum = 0,
   .alphaDenom = 0,
   .windowSize = 0,
   .clampData = { 0, 0 },
   .goodReadingCounterMax = 0,
   .badReadingCounterMax = 0,
   .enableExternalSensorCheck = false,
};

TEST_GROUP(ExternalFilteredSensorResolver)
{
   ExternalFilteredSensorResolver_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      ExternalFilteredSensorResolver_Init(
         &instance,
         dataModel,
         &config,
         &ambientSensorDataWithCheck);
   }

   void GivenTheModuleIsInitializedWithoutExternalTemperatureSupport()
   {
      ExternalFilteredSensorResolver_Init(
         &instance,
         dataModel,
         &config,
         &ambientSensorDataWithoutCheck);
   }

   void GivenTheInternalFilteredTemperatureIs(TemperatureDegFx100_t filteredTemperature)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_FilteredInternalTemperatureResolvedInDegFx100,
         &filteredTemperature);
   }

   void WhenTheInternalFilteredTemperatureChangesTo(TemperatureDegFx100_t filteredTemperature)
   {
      GivenTheInternalFilteredTemperatureIs(filteredTemperature);
   }

   void GivenTheInternalSensorStatusIs(bool status)
   {
      DataModel_Write(
         dataModel,
         Erd_AmbientThermistor_IsValidResolved,
         &status);
   }

   void WhenTheInternalSensorStatusChangesTo(bool status)
   {
      GivenTheInternalSensorStatusIs(status);
   }

   void GivenTheExternalFilteredTemperatureIs(TemperatureDegFx100_t externalFilteredTemperature)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_ExternalTemperatureInDegFx100,
         &externalFilteredTemperature);
   }

   void WhenTheExternalFilteredTemperatureChangesTo(TemperatureDegFx100_t externalFilteredTemperature)
   {
      GivenTheExternalFilteredTemperatureIs(externalFilteredTemperature);
   }

   void GivenTheExternalSensorStatusIs(bool status)
   {
      DataModel_Write(
         dataModel,
         Erd_Ambient_ExternalThermistorIsValid,
         &status);
   }

   void TheResolvedFilteredTemperatureShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(
         dataModel,
         Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheResolvedThermistorValidShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_Ambient_ResolvedThermistorIsValid,
         &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(ExternalFilteredSensorResolver, ShouldWriteExternalSensorTemperatureAndExternalSensorIsValidIntoResolvedErdsWhenInternalSensorIsNotValid)
{
   GivenTheInternalFilteredTemperatureIs(InternalTemperatureInDegFx100);
   GivenTheInternalSensorStatusIs(Invalid);
   GivenTheExternalFilteredTemperatureIs(ExternalTemperatureInDegFx100);
   GivenTheExternalSensorStatusIs(Valid);
   GivenTheModuleIsInitialized();

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100);
}

TEST(ExternalFilteredSensorResolver, ShouldWriteInternalSensorTemperatureAndInternalSensorIsValidIntoResolvedErdsWhenExternalSensorIsNotValid)
{
   GivenTheInternalFilteredTemperatureIs(InternalTemperatureInDegFx100);
   GivenTheInternalSensorStatusIs(Valid);
   GivenTheExternalFilteredTemperatureIs(ExternalTemperatureInDegFx100);
   GivenTheExternalSensorStatusIs(Invalid);
   GivenTheModuleIsInitialized();
   TheResolvedThermistorValidShouldBe(Valid);

   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100);
}

TEST(ExternalFilteredSensorResolver, ShouldWriteExternalSensorErdsToResolvedErdsWhenInternalSensorChangesToInvalid)
{
   GivenTheInternalSensorStatusIs(Valid);
   GivenTheExternalFilteredTemperatureIs(ExternalTemperatureInDegFx100);
   GivenTheInternalFilteredTemperatureIs(InternalTemperatureInDegFx100);
   GivenTheExternalSensorStatusIs(Valid);
   GivenTheModuleIsInitialized();

   WhenTheInternalSensorStatusChangesTo(Invalid);
   WhenTheInternalFilteredTemperatureChangesTo(SomeInvalidInternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100);
}

TEST(ExternalFilteredSensorResolver, ShouldWriteInternalSensorErdsToResolvedErdsWhenInternalSensorChangesToValid)
{
   GivenTheInternalSensorStatusIs(Invalid);
   GivenTheExternalSensorStatusIs(Valid);
   GivenTheExternalFilteredTemperatureIs(ExternalTemperatureInDegFx100);
   GivenTheInternalFilteredTemperatureIs(SomeInvalidInternalTemperatureInDegFx100);
   GivenTheModuleIsInitialized();

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100);

   WhenTheInternalSensorStatusChangesTo(Valid);
   WhenTheInternalFilteredTemperatureChangesTo(InternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100);
}

TEST(ExternalFilteredSensorResolver, ShouldWriteInternalSensorErdsToResolvedErdsWhenInternalSensorStatusIsValidAndTemperatureConstantlyChanges)
{
   GivenTheInternalSensorStatusIs(Invalid);
   GivenTheExternalSensorStatusIs(Valid);
   GivenTheExternalFilteredTemperatureIs(ExternalTemperatureInDegFx100);
   GivenTheInternalFilteredTemperatureIs(SomeInvalidInternalTemperatureInDegFx100);
   GivenTheModuleIsInitialized();

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100);

   WhenTheInternalSensorStatusChangesTo(Valid);
   WhenTheInternalFilteredTemperatureChangesTo(InternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100);

   WhenTheInternalFilteredTemperatureChangesTo(InternalTemperatureInDegFx100 + 1);
   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100 + 1);

   WhenTheInternalFilteredTemperatureChangesTo(InternalTemperatureInDegFx100 + 2);
   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100 + 2);
}

TEST(ExternalFilteredSensorResolver, ShouldWriteExternalSensorErdsToResolvedErdsWhenInternalSensorStatusIsInvalidAndExternalSensorIsValidAndTemperatureConstantlyChanges)
{
   GivenTheInternalSensorStatusIs(Valid);
   GivenTheExternalFilteredTemperatureIs(ExternalTemperatureInDegFx100);
   GivenTheInternalFilteredTemperatureIs(InternalTemperatureInDegFx100);
   GivenTheExternalSensorStatusIs(Valid);
   GivenTheModuleIsInitialized();

   WhenTheInternalSensorStatusChangesTo(Invalid);
   WhenTheInternalFilteredTemperatureChangesTo(SomeInvalidInternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100);

   WhenTheExternalFilteredTemperatureChangesTo(ExternalTemperatureInDegFx100 + 1);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100 + 1);

   WhenTheExternalFilteredTemperatureChangesTo(ExternalTemperatureInDegFx100 + 2);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100 + 2);
}

TEST(ExternalFilteredSensorResolver, ShouldNotWriteInternalSensorErdsToResolvedErdsWhenInternalTemperatureChangesAndItsInvalid)
{
   GivenTheInternalSensorStatusIs(Valid);
   GivenTheExternalFilteredTemperatureIs(ExternalTemperatureInDegFx100);
   GivenTheInternalFilteredTemperatureIs(InternalTemperatureInDegFx100);
   GivenTheExternalSensorStatusIs(Valid);
   GivenTheModuleIsInitialized();

   WhenTheInternalSensorStatusChangesTo(Invalid);
   WhenTheInternalFilteredTemperatureChangesTo(SomeInvalidInternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100);

   WhenTheInternalFilteredTemperatureChangesTo(InternalTemperatureInDegFx100 + 1);
   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100);
}

TEST(ExternalFilteredSensorResolver, ShouldNotWriteExternalSensorErdsToResolvedErdsWhenExternalTemperatureChangesAndTheInternalTemperatureIsValid)
{
   GivenTheInternalSensorStatusIs(Invalid);
   GivenTheExternalSensorStatusIs(Valid);
   GivenTheExternalFilteredTemperatureIs(ExternalTemperatureInDegFx100);
   GivenTheInternalFilteredTemperatureIs(SomeInvalidInternalTemperatureInDegFx100);
   GivenTheModuleIsInitialized();

   TheResolvedFilteredTemperatureShouldBe(ExternalTemperatureInDegFx100);
   TheResolvedThermistorValidShouldBe(Valid);

   WhenTheInternalSensorStatusChangesTo(Valid);
   WhenTheInternalFilteredTemperatureChangesTo(InternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100);

   WhenTheExternalFilteredTemperatureChangesTo(ExternalTemperatureInDegFx100 + 1);
   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100);
}

TEST(ExternalFilteredSensorResolver, ShouldNotWriteExternalSensorErdsToResolvedErdsIfCheckIsDisabled)
{
   GivenTheExternalSensorStatusIs(Valid);
   GivenTheInternalSensorStatusIs(Valid);
   GivenTheInternalFilteredTemperatureIs(InternalTemperatureInDegFx100);
   GivenTheModuleIsInitializedWithoutExternalTemperatureSupport();

   WhenTheInternalSensorStatusChangesTo(Valid);
   WhenTheInternalFilteredTemperatureChangesTo(InternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100);

   WhenTheExternalFilteredTemperatureChangesTo(ExternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Valid);
   TheResolvedFilteredTemperatureShouldBe(InternalTemperatureInDegFx100);

   WhenTheInternalSensorStatusChangesTo(Invalid);
   WhenTheInternalFilteredTemperatureChangesTo(SomeInvalidInternalTemperatureInDegFx100);

   TheResolvedThermistorValidShouldBe(Invalid);
   TheResolvedFilteredTemperatureShouldBe(SomeInvalidInternalTemperatureInDegFx100);

   WhenTheExternalFilteredTemperatureChangesTo(ExternalTemperatureInDegFx100 + 1);

   TheResolvedThermistorValidShouldBe(Invalid);
   TheResolvedFilteredTemperatureShouldBe(SomeInvalidInternalTemperatureInDegFx100);
}

TEST(ExternalFilteredSensorResolver, ShouldUseInternalSensorValuesIfBothSensorsAreInValid)
{
   GivenTheInternalSensorStatusIs(Invalid);
   GivenTheExternalSensorStatusIs(Invalid);
   GivenTheExternalFilteredTemperatureIs(SomeInvalidExternalTemperatureInDegFx100);
   GivenTheInternalFilteredTemperatureIs(SomeInvalidInternalTemperatureInDegFx100);
   GivenTheModuleIsInitialized();

   TheResolvedFilteredTemperatureShouldBe(SomeInvalidInternalTemperatureInDegFx100);
   TheResolvedThermistorValidShouldBe(Invalid);
}
