/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceCabinetGammaCalculator.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   FreezerTemperature = 400,
   IceCabinetTemperature = 420,
   FreezerAdjustedSetpoint = 1000,
   IceCabinetAdjustedSetpoint = 425,
   AnotherFreezerTemperature = 1000,
   AnotherIceCabinetTemperature = 200,
   AnotherFreezerAdjustedSetpoint = 3000,
   AnotherIceCabinetAdjustedSetpoint = 400,
   SomeFreezerTemperature = 2000,
   SomeIceCabinetTemperature = 500,
   SomeFreezerAdjustedSetpoint = 300,
   SomeIceCabinetAdjustedSetpoint = 300
};

static const IceCabinetGammaCalculatorConfig_t config = {
   .freezerFilteredTemperatureResolvedInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .iceCabinetTemperatureResolvedInDegFx100Erd = Erd_IceCabinet_FilteredTemperatureResolvedInDegFx100,
   .freezerAdjustedSetpointErd = Erd_Freezer_AdjustedSetpointInDegFx100,
   .iceCabinetAdjustedSetpointErd = Erd_IceCabinet_AdjustedSetpointInDegFx100,
   .gammaErd = Erd_IceCabinetGamma
};

static const CrossAmbientHysteresisAdjustmentData_t crossAmbientAdjustment = {
   .multiplier = 0,
   .divider = 1,
};

static const DeltaGridLineData_t iceCabinetGridLineData[] = {
   {
      .gridLinesDegFx100 = -150,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = -50,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = -20,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = 0,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = 25,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = 50,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = 150,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
};

static const DeltaGridLineData_t freezerGridLineData[] = {
   {
      .gridLinesDegFx100 = -1000,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = -800,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = -500,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = 0,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = 30,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = 80,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
   {
      .gridLinesDegFx100 = 100,
      .correction = 0,
      .crossAmbientAdjustment = &crossAmbientAdjustment,
   },
};

static const DeltaAxisGridLines_t iceCabinetAxisGridLineData = {
   .numberOfLines = NUM_ELEMENTS(iceCabinetGridLineData),
   .gridLineData = iceCabinetGridLineData
};

static const DeltaAxisGridLines_t freezerAxisGridLineData = {
   .numberOfLines = NUM_ELEMENTS(freezerGridLineData),
   .gridLineData = freezerGridLineData
};

TEST_GROUP(IceCabinetGammaCalculator)
{
   IceCabinetGammaCalculator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenGammaCalculatorIsInitialized()
   {
      IceCabinetGammaCalculator_Init(
         &instance,
         dataModel,
         &config,
         &freezerAxisGridLineData,
         &iceCabinetAxisGridLineData);
   }

   void GivenFreezerTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenFreezerTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenFreezerTemperatureIs(temperature);
   }

   void GivenIceCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_IceCabinet_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenIceCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenIceCabinetTemperatureIs(temperature);
   }

   void GivenFreezerAdjustedSetpointIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_AdjustedSetpointInDegFx100, &temperature);
   }

   void WhenFreezerAdjustedSetpointIs(TemperatureDegFx100_t temperature)
   {
      GivenFreezerAdjustedSetpointIs(temperature);
   }

   void GivenIceCabinetAdjustedSetpointIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_IceCabinet_AdjustedSetpointInDegFx100, &temperature);
   }

   void WhenIceCabinetAdjustedSetpointIs(TemperatureDegFx100_t temperature)
   {
      GivenIceCabinetAdjustedSetpointIs(temperature);
   }

   void GammaShouldBe(Gamma_t expected)
   {
      Gamma_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetGamma, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(IceCabinetGammaCalculator, ShouldInitialize)
{
   GivenGammaCalculatorIsInitialized();
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaOnInit)
{
   GivenFreezerTemperatureIs(FreezerTemperature);
   GivenIceCabinetTemperatureIs(IceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(FreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(IceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t freezerHysteresisRatio = (100 * (FreezerTemperature - (FreezerAdjustedSetpoint + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   int16_t iceCabinetHysteresisRatio = (100 * (IceCabinetTemperature - (IceCabinetAdjustedSetpoint + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - iceCabinetHysteresisRatio)) / 2);
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaOnInitAndClampAicToZero)
{
   GivenFreezerTemperatureIs(FreezerTemperature);
   GivenIceCabinetTemperatureIs(AnotherIceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(FreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(AnotherIceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t freezerHysteresisRatio = (100 * (FreezerTemperature - (FreezerAdjustedSetpoint + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - 0)) / 2);
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaOnInitAndClampAfcToZero)
{
   GivenFreezerTemperatureIs(AnotherFreezerTemperature);
   GivenIceCabinetTemperatureIs(IceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(AnotherFreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(IceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t iceCabinetHysteresisRatio = (100 * (IceCabinetTemperature - (IceCabinetAdjustedSetpoint + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (0 - iceCabinetHysteresisRatio)) / 2);
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaOnInitAndClampAicTo100)
{
   GivenFreezerTemperatureIs(FreezerTemperature);
   GivenIceCabinetTemperatureIs(SomeIceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(FreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(SomeIceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t freezerHysteresisRatio = (100 * (FreezerTemperature - (FreezerAdjustedSetpoint + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - 100)) / 2);
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaOnInitAndClampAfcTo100)
{
   GivenFreezerTemperatureIs(SomeFreezerTemperature);
   GivenIceCabinetTemperatureIs(IceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(SomeFreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(IceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t iceCabinetHysteresisRatio = (100 * (IceCabinetTemperature - (IceCabinetAdjustedSetpoint + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (100 - iceCabinetHysteresisRatio)) / 2);
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaWhenFreezerTemperatureChanges)
{
   GivenFreezerTemperatureIs(FreezerTemperature);
   GivenIceCabinetTemperatureIs(IceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(FreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(IceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t freezerHysteresisRatio = (100 * (FreezerTemperature - (FreezerAdjustedSetpoint + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   int16_t iceCabinetHysteresisRatio = (100 * (IceCabinetTemperature - (IceCabinetAdjustedSetpoint + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - iceCabinetHysteresisRatio)) / 2);

   WhenFreezerTemperatureIs(FreezerTemperature + 100);

   int16_t aFzUpdated = (100 * ((FreezerTemperature + 100) - (FreezerAdjustedSetpoint + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (aFzUpdated - iceCabinetHysteresisRatio)) / 2);
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaWhenIceCabinetTemperatureChanges)
{
   GivenFreezerTemperatureIs(FreezerTemperature);
   GivenIceCabinetTemperatureIs(IceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(FreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(IceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t freezerHysteresisRatio = (100 * (FreezerTemperature - (FreezerAdjustedSetpoint + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   int16_t iceCabinetHysteresisRatio = (100 * (IceCabinetTemperature - (IceCabinetAdjustedSetpoint + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - iceCabinetHysteresisRatio)) / 2);

   WhenIceCabinetTemperatureIs(IceCabinetTemperature + 10);

   int16_t aIcUpdated = (100 * ((IceCabinetTemperature + 10) - (IceCabinetAdjustedSetpoint + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - aIcUpdated)) / 2);
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaWhenFreezerAdjustedSetpointChanges)
{
   GivenFreezerTemperatureIs(FreezerTemperature);
   GivenIceCabinetTemperatureIs(IceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(FreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(IceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t freezerHysteresisRatio = (100 * (FreezerTemperature - (FreezerAdjustedSetpoint + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   int16_t iceCabinetHysteresisRatio = (100 * (IceCabinetTemperature - (IceCabinetAdjustedSetpoint + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - iceCabinetHysteresisRatio)) / 2);

   WhenFreezerAdjustedSetpointIs(FreezerAdjustedSetpoint + 100);

   int16_t aFzUpdated = (100 * (FreezerTemperature - ((FreezerAdjustedSetpoint + 100) + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (aFzUpdated - iceCabinetHysteresisRatio)) / 2);
}

TEST(IceCabinetGammaCalculator, ShouldCalculateGammaWhenIceCabinetAdjustedSetpointChanges)
{
   GivenFreezerTemperatureIs(FreezerTemperature);
   GivenIceCabinetTemperatureIs(IceCabinetTemperature);
   GivenFreezerAdjustedSetpointIs(FreezerAdjustedSetpoint);
   GivenIceCabinetAdjustedSetpointIs(IceCabinetAdjustedSetpoint);
   GivenGammaCalculatorIsInitialized();

   int16_t freezerHysteresisRatio = (100 * (FreezerTemperature - (FreezerAdjustedSetpoint + freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
      (freezerAxisGridLineData.gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 - freezerAxisGridLineData.gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100));

   int16_t iceCabinetHysteresisRatio = (100 * (IceCabinetTemperature - (IceCabinetAdjustedSetpoint + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - iceCabinetHysteresisRatio)) / 2);

   WhenIceCabinetAdjustedSetpointIs(IceCabinetAdjustedSetpoint + 10);

   int16_t aIcUpdated = (100 * (IceCabinetTemperature - ((IceCabinetAdjustedSetpoint + 10) + iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
      (iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 - iceCabinetAxisGridLineData.gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100));

   GammaShouldBe((100 - (freezerHysteresisRatio - aIcUpdated)) / 2);
}
