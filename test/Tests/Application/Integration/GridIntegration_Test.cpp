/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "CalculatedGridLines.h"
#include "DeltaGridLines.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"

#define Given
#define After
#define When

enum
{
   Valid = true,
   Invalid = false
};

enum
{
   NumberOfGridBlocksPerAxis = NumberOfGridLinesPerAxis + 1,
   NumberOfGridBlocks = NumberOfGridBlocksPerAxis * NumberOfGridBlocksPerAxis
};

typedef struct
{
   TemperatureDegFx100_t freezerTemperature;
   TemperatureDegFx100_t freshFoodTemperature;
} GridBlockTemperatures;

static const FanControl_t fanSpeedOff = {
   .type = FanControlType_DutyCycle,
   .dutyCycle = 0
};

TEST_GROUP(GridIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   GridBlockTemperatures gridBlockTemperatures[NumberOfGridBlocks];
   const FanCareAboutCoolingModeSpeedData_t *condenserFanSpeedData;
   const FanCareAboutCoolingModeSpeedData_t *freezerEvapFanSpeedData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelDouble.dataModel;

      condenserFanSpeedData = &PersonalityParametricData_Get(dataModel)->fanData->condenserFan.careAboutCoolingModeSpeedData;
      freezerEvapFanSpeedData = &PersonalityParametricData_Get(dataModel)->fanData->freezerEvapFan.careAboutCoolingModeSpeedData;
   }

   void ApplicationHasBeenInitialized()
   {
      Application_Init(&instance, dataModel, resetReason);

      GridBlockTemperaturesHaveBeenGenerated();

      DataModel_Write(dataModel, Erd_GridOverrideEnable, set);
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureOverrideRequest, set);
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureOverrideRequest, set);
      DataModel_Write(dataModel, Erd_DisableMinimumCompressorTimes, set);
   }

   void GridBlockTemperaturesHaveBeenGenerated()
   {
      CalculatedGridLines_t calculatedGridLines;
      DataModel_Read(dataModel, Erd_Grid_CalculatedGridLines, &calculatedGridLines);

      gridBlockTemperatures[0].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1;
      gridBlockTemperatures[0].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[1].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1;
      gridBlockTemperatures[1].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[2].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1;
      gridBlockTemperatures[2].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta] - 1;
      gridBlockTemperatures[3].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1;
      gridBlockTemperatures[3].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[4].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1;
      gridBlockTemperatures[4].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[5].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1;
      gridBlockTemperatures[5].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[6].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1;
      gridBlockTemperatures[6].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;

      gridBlockTemperatures[7].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh] + 1;
      gridBlockTemperatures[7].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[8].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh] + 1;
      gridBlockTemperatures[8].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[9].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh] + 1;
      gridBlockTemperatures[9].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta] - 1;
      gridBlockTemperatures[10].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh] + 1;
      gridBlockTemperatures[10].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[11].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh] + 1;
      gridBlockTemperatures[11].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[12].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh] + 1;
      gridBlockTemperatures[12].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[13].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh] + 1;
      gridBlockTemperatures[13].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;

      gridBlockTemperatures[14].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh] + 1;
      gridBlockTemperatures[14].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[15].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh] + 1;
      gridBlockTemperatures[15].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[16].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh] + 1;
      gridBlockTemperatures[16].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta] - 1;
      gridBlockTemperatures[17].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh] + 1;
      gridBlockTemperatures[17].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[18].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh] + 1;
      gridBlockTemperatures[18].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[19].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh] + 1;
      gridBlockTemperatures[19].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[20].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh] + 1;
      gridBlockTemperatures[20].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;

      gridBlockTemperatures[21].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[21].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[22].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[22].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[23].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[23].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta] - 1;
      gridBlockTemperatures[24].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[24].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[25].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[25].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[26].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[26].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[27].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[27].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;

      gridBlockTemperatures[28].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta] + 1;
      gridBlockTemperatures[28].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[29].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta] + 1;
      gridBlockTemperatures[29].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[30].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta] + 1;
      gridBlockTemperatures[30].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta] - 1;
      gridBlockTemperatures[31].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta] + 1;
      gridBlockTemperatures[31].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[32].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta] + 1;
      gridBlockTemperatures[32].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[33].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta] + 1;
      gridBlockTemperatures[33].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[34].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta] + 1;
      gridBlockTemperatures[34].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;

      gridBlockTemperatures[35].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[35].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[36].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[36].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[37].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[37].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta] - 1;
      gridBlockTemperatures[38].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[38].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[39].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[39].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[40].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[40].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[41].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[41].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;

      gridBlockTemperatures[42].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[42].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[43].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[43].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[44].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[44].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta] - 1;
      gridBlockTemperatures[45].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[45].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[46].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[46].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[47].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[47].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[48].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[48].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;
   }

   void GridRuns()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_GridOverrideSignal);
   }

   void FreezerFilteredTemperatureIs(TemperatureDegFx100_t value)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100, &value);
   }

   void FreshFoodFilteredTemperatureIs(TemperatureDegFx100_t value)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureOverrideValueInDegFx100, &value);
   }

   void TheGridBlockNumberShouldBe(GridBlockNumber_t expected)
   {
      GridBlockNumber_t actual;
      DataModel_Read(dataModel, Erd_Grid_BlockNumber, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheCoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void TheCoolingModeShouldBe(CoolingMode_t expected)
   {
      CoolingMode_t actual;
      DataModel_Read(dataModel, Erd_CoolingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheCoolingSpeedIs(CoolingSpeed_t speed)
   {
      DataModel_Write(dataModel, Erd_CoolingSpeed, &speed);
   }

   void TheCoolingSpeedShouldBe(CoolingSpeed_t expected)
   {
      CoolingSpeed_t actual;
      DataModel_Read(dataModel, Erd_CoolingSpeed, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheGridAreaIs(GridArea_t gridArea)
   {
      DataModel_Write(dataModel, Erd_GridArea, &gridArea);
   }

   void TheGridAreaShouldBe(GridArea_t expected)
   {
      GridArea_t actual;
      DataModel_Read(dataModel, Erd_GridArea, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheSingleEvaporatorPulldownActiveIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SingleEvaporatorPulldownActive, &state);
   }

   void TheSingleEvaporatorPulldownActiveShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_SingleEvaporatorPulldownActive, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheCompressorRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorRelay, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheCalculatedCondenserFanControlShouldBe(FanControl_t expected)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedCondenserFanControl, &actual);

      CHECK_EQUAL(expected.dutyCycle, actual.dutyCycle);
   }

   void TheCalculatedFreezerEvapFanControlShouldBe(FanControl_t expected)
   {
      FanControl_t actual;
      DataModel_Read(dataModel, Erd_CalculatedFreezerEvapFanControl, &actual);

      CHECK_EQUAL(expected.dutyCycle, actual.dutyCycle);
   }

   void TheFreshFoodDamperStepperMotorDriveEnableShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerAndFreshFoodTemperaturesAreSetForBlock(uint8_t gridBlockId)
   {
      FreezerFilteredTemperatureIs(gridBlockTemperatures[gridBlockId].freezerTemperature);
      FreshFoodFilteredTemperatureIs(gridBlockTemperatures[gridBlockId].freshFoodTemperature);
   }

   void TheSingleEvapPulldownAndTripMitigationIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SingleEvaporatorPulldownActive, &state);
      DataModel_Write(dataModel, Erd_CompressorTripMitigationActive, &state);
   }

   void FreshFoodThermistorValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void BothThermistorsAreValid(void)
   {
      FreshFoodThermistorValidIs(Valid);
      FreezerThermistorValidIs(Valid);
   }
};

TEST(GridIntegration, ShouldInitialize)
{
   ApplicationHasBeenInitialized();
}

TEST(GridIntegration, ShouldScrollThroughEachGridBlock)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 0; gridBlockIndex < NumberOfGridBlocks; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks0and1)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 0; gridBlockIndex <= 1; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_FreshFood);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_2);
      When TheSingleEvaporatorPulldownActiveIs(CLEAR);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_PullDown);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.superHighSpeed);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks2and3)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 2; gridBlockIndex <= 3; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_2);
      When TheSingleEvaporatorPulldownActiveIs(CLEAR);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Off);
      TheCoolingSpeedShouldBe(CoolingSpeed_PullDown);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks4and5and6)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 4; gridBlockIndex <= 6; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvaporatorPulldownActiveIs(CLEAR);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_PullDown);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.superHighSpeed);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks7and8and14)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   uint8_t gridBlockNumbers[] = { 7, 8, 14 };
   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_2);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks9And10)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 9; gridBlockIndex <= 10; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheSingleEvapPulldownAndTripMitigationIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheSingleEvapPulldownAndTripMitigationIs(CLEAR);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks11And12And13)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 11; gridBlockIndex <= 13; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvapPulldownAndTripMitigationIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheGridAreaShouldBe(GridArea_2);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvapPulldownAndTripMitigationIs(CLEAR);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_2);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      When TheSingleEvaporatorPulldownActiveIs(CLEAR);
      After GridRuns();
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);

      When TheSingleEvaporatorPulldownActiveIs(SET);
      After GridRuns();
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks15)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_Off);
   Given TheGridAreaIs(GridArea_2);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(15);

   After GridRuns();
   TheGridBlockNumberShouldBe(15);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When TheCoolingModeIs(CoolingMode_Off);
   When TheCoolingSpeedIs(CoolingSpeed_High);
   When TheGridAreaIs(GridArea_2);
   When TheSingleEvaporatorPulldownActiveIs(SET);

   After GridRuns();
   TheGridBlockNumberShouldBe(15);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_High);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks16And17)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 16; gridBlockIndex <= 17; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_FreshFood);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      When TheCoolingModeIs(CoolingMode_FreshFood);
      When TheCoolingSpeedIs(CoolingSpeed_High);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks18)
{
   Given ApplicationHasBeenInitialized();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(18);
   When TheCoolingModeIs(CoolingMode_Off);
   When TheCoolingSpeedIs(CoolingSpeed_Off);
   When TheSingleEvapPulldownAndTripMitigationIs(SET);
   When BothThermistorsAreValid();

   After GridRuns();
   TheGridBlockNumberShouldBe(18);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_High);
   TheGridAreaShouldBe(GridArea_2);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When TheCoolingModeIs(CoolingMode_Off);
   When TheCoolingSpeedIs(CoolingSpeed_Off);
   When TheSingleEvapPulldownAndTripMitigationIs(CLEAR);

   After GridRuns();
   TheGridBlockNumberShouldBe(18);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheGridAreaShouldBe(GridArea_2);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks19And20And27And34And41)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   uint8_t gridBlockNumbers[] = { 19, 20, 27, 34, 41 };
   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks21)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_Off);
   Given TheGridAreaIs(GridArea_2);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(21);

   After GridRuns();
   TheGridBlockNumberShouldBe(21);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks22)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_Off);
   Given TheGridAreaIs(GridArea_2);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(22);

   After GridRuns();
   TheGridBlockNumberShouldBe(22);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When TheCoolingModeIs(CoolingMode_Off);
   When TheCoolingSpeedIs(CoolingSpeed_Low);
   When TheGridAreaIs(GridArea_2);
   When TheSingleEvaporatorPulldownActiveIs(SET);

   After GridRuns();
   TheGridBlockNumberShouldBe(22);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks23And24)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 23; gridBlockIndex <= 24; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_FreshFood);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      When TheCoolingModeIs(CoolingMode_FreshFood);
      When TheCoolingSpeedIs(CoolingSpeed_Mid);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks25And32And39)
{
   Given ApplicationHasBeenInitialized();
   Given BothThermistorsAreValid();

   uint8_t gridBlockNumbers[] = { 25, 32, 39 };
   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Mid);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks26And33And40)
{
   Given ApplicationHasBeenInitialized();

   uint8_t gridBlockNumbers[] = { 26, 33, 40 };
   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvaporatorPulldownActiveIs(SET);
      When BothThermistorsAreValid();

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_High);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks28And29And35)
{
   Given ApplicationHasBeenInitialized();

   uint8_t gridBlockNumbers[] = { 28, 29, 35 };
   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      When TheCoolingModeIs(CoolingMode_Off);
      When TheCoolingSpeedIs(CoolingSpeed_Off);
      When TheGridAreaIs(GridArea_2);
      When TheSingleEvaporatorPulldownActiveIs(SET);
      When BothThermistorsAreValid();

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks30)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_Off);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(30);

   After GridRuns();
   TheGridBlockNumberShouldBe(30);
   TheCoolingModeShouldBe(CoolingMode_Off);
   TheCoolingSpeedShouldBe(CoolingSpeed_Off);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(OFF);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks31)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_Off);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(31);

   After GridRuns();
   TheGridBlockNumberShouldBe(31);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Off);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(OFF);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_Low);
   Given TheSingleEvaporatorPulldownActiveIs(SET);

   After GridRuns();
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks36)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_FreshFood);
   Given TheCoolingSpeedIs(CoolingSpeed_High);
   Given TheGridAreaIs(GridArea_2);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(36);

   After GridRuns();
   TheGridBlockNumberShouldBe(36);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When TheCoolingModeIs(CoolingMode_FreshFood);
   When TheCoolingSpeedIs(CoolingSpeed_Off);
   When TheGridAreaIs(GridArea_2);
   When TheSingleEvaporatorPulldownActiveIs(SET);

   After GridRuns();
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Off);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(OFF);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks37)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_High);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(37);

   After GridRuns();
   TheGridBlockNumberShouldBe(37);
   TheCoolingModeShouldBe(CoolingMode_Off);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(OFF);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When TheCoolingModeIs(CoolingMode_Off);
   When TheCoolingSpeedIs(CoolingSpeed_Off);
   When TheSingleEvaporatorPulldownActiveIs(SET);

   After GridRuns();
   TheGridBlockNumberShouldBe(37);
   TheCoolingModeShouldBe(CoolingMode_Off);
   TheCoolingSpeedShouldBe(CoolingSpeed_Off);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(OFF);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks38)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Freezer);
   Given TheCoolingSpeedIs(CoolingSpeed_High);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(38);

   After GridRuns();
   TheGridBlockNumberShouldBe(38);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When TheCoolingModeIs(CoolingMode_FreshFood);
   When TheCoolingSpeedIs(CoolingSpeed_Off);
   When TheSingleEvaporatorPulldownActiveIs(SET);

   After GridRuns();
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Off);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(OFF);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks42And43)
{
   Given ApplicationHasBeenInitialized();

   for(uint8_t gridBlockIndex = 42; gridBlockIndex <= 43; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_FreshFood);
      When TheCoolingSpeedIs(CoolingSpeed_High);
      When TheGridAreaIs(GridArea_2);
      When TheSingleEvaporatorPulldownActiveIs(SET);
      When BothThermistorsAreValid();

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks44And45)
{
   Given ApplicationHasBeenInitialized();

   for(uint8_t gridBlockIndex = 44; gridBlockIndex <= 45; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_FreshFood);
      When TheCoolingSpeedIs(CoolingSpeed_High);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvaporatorPulldownActiveIs(SET);
      When BothThermistorsAreValid();

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      When TheCoolingModeIs(CoolingMode_Freezer);
      When TheCoolingSpeedIs(CoolingSpeed_Mid);
      When TheGridAreaIs(GridArea_2);
      When TheSingleEvaporatorPulldownActiveIs(SET);

      After GridRuns();
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks46And47)
{
   Given ApplicationHasBeenInitialized();

   for(uint8_t gridBlockIndex = 46; gridBlockIndex <= 47; gridBlockIndex++)
   {
      When FreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      When TheCoolingModeIs(CoolingMode_Freezer);
      When TheCoolingSpeedIs(CoolingSpeed_High);
      When TheGridAreaIs(GridArea_1);
      When TheSingleEvaporatorPulldownActiveIs(SET);
      When BothThermistorsAreValid();

      After GridRuns();
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheCompressorRelayShouldBe(ON);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks48)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_Off);
   Given TheGridAreaIs(GridArea_1);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(48);

   After GridRuns();
   TheGridBlockNumberShouldBe(48);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheGridAreaShouldBe(GridArea_2);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks48ThenChangeToBlock27LoadsWhenFreezerSensorBecomesInvalid)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Off);
   Given TheCoolingSpeedIs(CoolingSpeed_Off);
   Given TheGridAreaIs(GridArea_1);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(48);

   After GridRuns();
   TheGridBlockNumberShouldBe(48);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheGridAreaShouldBe(GridArea_2);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When FreezerThermistorValidIs(Invalid);

   After GridRuns();
   TheGridBlockNumberShouldBe(27);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_High);
   TheGridAreaShouldBe(GridArea_2);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks38ThenChangeToBlock36LoadsWhenFreshFoodSensorBecomesInvalid)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Freezer);
   Given TheCoolingSpeedIs(CoolingSpeed_High);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(38);

   After GridRuns();
   TheGridBlockNumberShouldBe(38);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When FreshFoodThermistorValidIs(Invalid);

   After GridRuns();
   TheGridBlockNumberShouldBe(36);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks38ThenTurnAllLoadsOffWhenBothSensorsBecomeInvalid)
{
   Given ApplicationHasBeenInitialized();
   Given TheCoolingModeIs(CoolingMode_Freezer);
   Given TheCoolingSpeedIs(CoolingSpeed_High);
   Given TheSingleEvaporatorPulldownActiveIs(SET);
   Given BothThermistorsAreValid();

   When FreezerAndFreshFoodTemperaturesAreSetForBlock(38);

   After GridRuns();
   TheGridBlockNumberShouldBe(38);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   When FreshFoodThermistorValidIs(Invalid);
   When FreezerThermistorValidIs(Invalid);

   After GridRuns();
   TheGridBlockNumberShouldBe(36);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheCompressorRelayShouldBe(OFF);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
}
