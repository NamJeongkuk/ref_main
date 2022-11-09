/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FreezerFilteredTemperatureTooWarmOnPowerUp.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   SixGridLines = 6,
   TwoDimensional = 2,
};

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .gridLinesDegFx100 = { 0, -450, 150, 450, 950, 1150 }
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .gridLinesDegFx100 = { -250, 0, 250, 600, 750, 5500 }
};

static CalculatedAxisGridLines_t calcGrid[] = { freshFoodCalcAxis, freezerCalcAxis };

static CalculatedGridLines_t calcGridLines = {
   .freshFoodGridLine = freshFoodCalcAxis,
   .freezerGridLine = freezerCalcAxis
};

TEST_GROUP(FreezerFilteredTemperatureTooWarmOnPowerUp)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   const DefrostData_t *defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpIsInitialized()
   {
      FreezerFilteredTemperatureTooWarmOnPowerUp_Init(dataModel);
   }
};

TEST(FreezerFilteredTemperatureTooWarmOnPowerUp, ShouldSetErdToTrueWhenFreezerFilteredTemperatureIsAboveGridFreezerExtremeHysteresis)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIsInitialized();

   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(FreezerFilteredTemperatureTooWarmOnPowerUp, ShouldSetErdToFalseWhenFreezerFilteredTemperatureIsEqualToGridFreezerExtremeHysteresis)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh]);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIsInitialized();

   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(FreezerFilteredTemperatureTooWarmOnPowerUp, ShouldSetErdToTrueWhenFreezerFilteredTemperatureIsAboveFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData->freezerDefrostTerminationTemperatureInDegFx100 + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIsInitialized();

   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(FreezerFilteredTemperatureTooWarmOnPowerUp, ShouldSetErdToTrueWhenFreezerFilteredTemperatureIsEqualToFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData->freezerDefrostTerminationTemperatureInDegFx100);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIsInitialized();

   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}
