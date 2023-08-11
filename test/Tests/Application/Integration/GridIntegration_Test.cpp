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
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"

enum
{
   Valid = true,
   Invalid = false
};

enum
{
   Closed = 0,
   Open = 1
};

enum
{
   AnHourInMillisecondsToAllowAmbientWindowAverageTemperatureToChange = 60 * MSEC_PER_MIN
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
   TimerModule_TestDouble_t *timerModuleTestDouble;
   ResetReason_t resetReason;
   GridBlockTemperatures gridBlockTemperatures[NumberOfGridBlocks];
   const GridData_t *gridData;
   const CompressorData_t *compressorData;
   const FanCareAboutCoolingModeSpeedData_t *condenserFanSpeedData;
   const FanCareAboutCoolingModeSpeedData_t *freezerEvapFanSpeedData;
   const LoadOffDoorOpenData_t *loadOffDoorOpenData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      gridData = PersonalityParametricData_Get(dataModel)->gridData;
      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
      condenserFanSpeedData = PersonalityParametricData_Get(dataModel)->fanData->condenserFan.careAboutCoolingModeSpeedData;
      freezerEvapFanSpeedData = PersonalityParametricData_Get(dataModel)->fanData->freezerEvapFan.careAboutCoolingModeSpeedData;
      loadOffDoorOpenData = PersonalityParametricData_Get(dataModel)->loadOffDoorOpenData;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(&instance, dataModel, resetReason);

      GridBlockTemperaturesHaveBeenGenerated();

      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureOverrideRequest, set);
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureOverrideRequest, set);
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
      gridBlockTemperatures[2].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSetpointDelta] - 1;
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
      gridBlockTemperatures[9].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSetpointDelta] - 1;
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
      gridBlockTemperatures[16].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSetpointDelta] - 1;
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
      gridBlockTemperatures[23].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSetpointDelta] - 1;
      gridBlockTemperatures[24].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[24].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[25].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[25].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[26].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[26].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[27].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst] + 1;
      gridBlockTemperatures[27].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;

      gridBlockTemperatures[28].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSetpointDelta] + 1;
      gridBlockTemperatures[28].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[29].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSetpointDelta] + 1;
      gridBlockTemperatures[29].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[30].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSetpointDelta] + 1;
      gridBlockTemperatures[30].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSetpointDelta] - 1;
      gridBlockTemperatures[31].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSetpointDelta] + 1;
      gridBlockTemperatures[31].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[32].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSetpointDelta] + 1;
      gridBlockTemperatures[32].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[33].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSetpointDelta] + 1;
      gridBlockTemperatures[33].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[34].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerSetpointDelta] + 1;
      gridBlockTemperatures[34].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;

      gridBlockTemperatures[35].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[35].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl] - 1;
      gridBlockTemperatures[36].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[36].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst] - 1;
      gridBlockTemperatures[37].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] + 1;
      gridBlockTemperatures[37].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSetpointDelta] - 1;
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
      gridBlockTemperatures[44].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSetpointDelta] - 1;
      gridBlockTemperatures[45].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[45].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst] - 1;
      gridBlockTemperatures[46].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[46].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh] - 1;
      gridBlockTemperatures[47].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[47].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] - 1;
      gridBlockTemperatures[48].freezerTemperature = calculatedGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst] - 1;
      gridBlockTemperatures[48].freshFoodTemperature = calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh] + 1;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
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

   void GivenTheCoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void WhenTheCoolingModeIs(CoolingMode_t mode)
   {
      GivenTheCoolingModeIs(mode);
   }

   void TheCoolingModeShouldBe(CoolingMode_t expected)
   {
      CoolingMode_t actual;
      DataModel_Read(dataModel, Erd_CoolingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheCoolingSpeedIs(CoolingSpeed_t speed)
   {
      DataModel_Write(dataModel, Erd_CoolingSpeed, &speed);
   }

   void WhenTheCoolingSpeedIs(CoolingSpeed_t speed)
   {
      GivenTheCoolingSpeedIs(speed);
   }

   void TheCoolingSpeedShouldBe(CoolingSpeed_t expected)
   {
      CoolingSpeed_t actual;
      DataModel_Read(dataModel, Erd_CoolingSpeed, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheGridAreaIs(GridArea_t gridArea)
   {
      DataModel_Write(dataModel, Erd_GridArea, &gridArea);
   }

   void WhenTheGridAreaIs(GridArea_t gridArea)
   {
      GivenTheGridAreaIs(gridArea);
   }

   void TheGridAreaShouldBe(GridArea_t expected)
   {
      GridArea_t actual;
      DataModel_Read(dataModel, Erd_GridArea, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheSingleEvaporatorPulldownActiveIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SingleEvaporatorPulldownActive, &state);
   }

   void WhenTheSingleEvaporatorPulldownActiveIs(bool state)
   {
      GivenTheSingleEvaporatorPulldownActiveIs(state);
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

   void CompressorStateShouldBe(CompressorState_t expected)
   {
      CompressorState_t actual;
      DataModel_Read(dataModel, Erd_CompressorState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CompressorGridVoteShouldBe(CompressorSpeed_t expectedSpeed, Vote_t expectedCare)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_GridVote, &actual);

      CHECK_EQUAL(expectedSpeed, actual.speed);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void CompressorResolvedVoteShouldBe(CompressorSpeed_t expectedSpeed, Vote_t expectedCare)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_ResolvedVote, &actual);

      CHECK_EQUAL(expectedSpeed, actual.speed);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void CompressorControllerSpeedRequestShouldBe(CompressorSpeed_t expected)
   {
      CompressorSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorControllerSpeedRequest, &actual);

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

   void LoadOffDoorOpenVoteForFreezerEvapShouldBe(FanSpeed_t speed, Vote_t care)
   {
      FanVotedSpeed_t vote;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_LoadOffDoorOpenVote, &vote);

      CHECK_EQUAL(speed, vote.speed);
      CHECK_EQUAL(care, vote.care);
   }

   void TheFreshFoodDamperStepperMotorDriveEnableShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DamperPositionShouldBe(DamperPosition_t expected)
   {
      DamperPosition_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperCurrentPosition, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GridShouldVoteForDamperToBe(DamperVotedPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_GridVote, &actual);

      CHECK_EQUAL(expected.position, actual.position);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(uint8_t gridBlockId)
   {
      FreezerFilteredTemperatureIs(gridBlockTemperatures[gridBlockId].freezerTemperature);
      FreshFoodFilteredTemperatureIs(gridBlockTemperatures[gridBlockId].freshFoodTemperature);
   }

   void WhenCompressorTripMitigationIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorTripMitigationActive, &state);
   }

   void WhenFreshFoodThermistorValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void WhenFreezerThermistorValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void GivenBothThermistorsAreValid(void)
   {
      WhenFreshFoodThermistorValidIs(Valid);
      WhenFreezerThermistorValidIs(Valid);
   }

   void WhenBothThermistorsAreValid(void)
   {
      GivenBothThermistorsAreValid();
   }

   void GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors()
   {
      ReferDataModel_TestDouble_Reset(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);

      GivenApplicationHasBeenInitialized();
      GivenBothThermistorsAreValid();
   }

   void CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(FanControl_t condenserFanControl, FanControl_t evaporatorFanControl)
   {
      After(compressorData->compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
      TheCompressorRelayShouldBe(OFF);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(condenserFanControl);
      TheCalculatedFreezerEvapFanControlShouldBe(evaporatorFanControl);
      TheCompressorRelayShouldBe(ON);
   }

   void CompressorRelayShouldBeOnAndFansAtStartupSpeedDuringStartupTime()
   {
      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.superHighSpeed);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   }

   void CompressorRelayShouldBeOnAndFansAtStartupSpeedDuringStartupTimeThenFansShouldTurnOff()
   {
      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   }

   void WhenTheFreshFoodAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_AdjustedSetpointInDegFx100,
         &setpoint);
   }

   void FreshFoodAdjustedSetpointShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel,
         Erd_FreshFood_AdjustedSetpointInDegFx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheFreezerAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_AdjustedSetpointInDegFx100,
         &setpoint);
   }

   void GivenTheAdjustedSetpointsAre(
      TemperatureDegFx100_t freshFoodAdjustedSetpoint,
      TemperatureDegFx100_t freezerAdjustedSetpoint)
   {
      WhenTheFreshFoodAdjustedSetpointIs(freshFoodAdjustedSetpoint);
      WhenTheFreezerAdjustedSetpointIs(freezerAdjustedSetpoint);
   }

   void FreezerAdjustedSetpointShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel,
         Erd_Freezer_AdjustedSetpointInDegFx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheFreshFoodSumOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100,
         &offset);
   }

   void WhenTheFreezerSumOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100,
         &offset);
   }

   void FreshFoodCabinetOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_CabinetOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodCrossAmbientOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_CrossAmbientOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodCabinetPlusCrossAmbientOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodHighAmbientOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_HighAmbientOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodThermalShiftShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_ThermalShiftInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodSetpointOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_SetpointOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodPulldownOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_PulldownOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodResolvedSetpointShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFood_ResolvedSetpointInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerResolvedSetpointShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_Freezer_ResolvedSetpointInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerCabinetOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_Freezer_CabinetOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerCrossAmbientOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_Freezer_CrossAmbientOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerCabinetPlusCrossAmbientOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerHighAmbientOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_Freezer_HighAmbientOffsetInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerThermalShiftShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_Freezer_ThermalShiftInDegFx100, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheOffsetsAre(
      TemperatureDegFx100_t freshFoodOffset,
      TemperatureDegFx100_t freezerOffset)
   {
      WhenTheFreshFoodSumOffsetIs(freshFoodOffset);
      WhenTheFreezerSumOffsetIs(freezerOffset);
   }

   void GivenTheFreezerThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_ThermalShiftInDegFx100,
         &shift);
   }

   void WhenTheFreezerThermalShiftIs(TemperatureDegFx100_t shift)
   {
      GivenTheFreezerThermalShiftIs(shift);
   }

   void WhenTheAmbientTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Ambient_FilteredInternalTemperatureOverrideRequest, set);
      DataModel_Write(dataModel, Erd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100, &temperature);
   }

   void GivenTheAmbientTemperatureIs(TemperatureDegFx100_t temperature)
   {
      WhenTheAmbientTemperatureChangesTo(temperature);
   }

   void AmbientWindowAverageTemperatureShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(
         dataModel,
         Erd_Ambient_WindowAveragedTemperatureInDegFx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CrossAmbientHysteresisAdjustmentShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(
         dataModel,
         Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheCrossAmbientHysteresisAdjustmentChangesTo(TemperatureDegFx100_t adjustment)
   {
      DataModel_Write(
         dataModel,
         Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
         &adjustment);
   }

   void GivenTheCrossAmbientHysteresisAdjustmentIs(TemperatureDegFx100_t adjustment)
   {
      DataModel_Write(
         dataModel,
         Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
         &adjustment);
   }

   void GivenTheFreshFoodThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(
         dataModel,
         Erd_FreshFood_ThermalShiftInDegFx100,
         &shift);
   }

   void WhenTheFreshFoodThermalShiftIs(TemperatureDegFx100_t shift)
   {
      GivenTheFreshFoodThermalShiftIs(shift);
   }

   void GivenTheShiftsAre(
      TemperatureDegFx100_t freshFoodShift,
      TemperatureDegFx100_t freezerShift)
   {
      GivenTheFreshFoodThermalShiftIs(freshFoodShift);
      GivenTheFreezerThermalShiftIs(freezerShift);
   }

   void WhenTheFreshFoodRawSetpointIs(TemperatureDegFx100_t rawSetpoint)
   {
      SetpointVotedTemperature_t temp = {
         .temperatureInDegFx100 = rawSetpoint,
         .care = true
      };

      DataModel_Write(dataModel,
         Erd_FreshFoodSetpoint_ResolvedVote,
         &temp);
   }

   void WhenTheFreezerRawSetpointIs(TemperatureDegFx100_t rawSetpoint)
   {
      SetpointVotedTemperature_t temp = {
         .temperatureInDegFx100 = rawSetpoint,
         .care = true
      };

      DataModel_Write(dataModel,
         Erd_FreezerSetpoint_ResolvedVote,
         &temp);
   }

   void GivenTheRawSetpointsAre(
      TemperatureDegFx100_t freshFoodRawSetpoint,
      TemperatureDegFx100_t freezerRawSetpoint)
   {
      WhenTheFreshFoodRawSetpointIs(freshFoodRawSetpoint);
      WhenTheFreezerRawSetpointIs(freezerRawSetpoint);
   }

   void TheCalculatedGridLineTempShouldBe(
      TemperatureDegFx100_t temperature,
      uint8_t dimension,
      uint8_t gridLineIndex)
   {
      CalculatedGridLines_t calcLines;
      DataModel_Read(dataModel,
         Erd_Grid_CalculatedGridLines,
         &calcLines);

      if(dimension == GridDelta_FreshFood)
      {
         CHECK_EQUAL(temperature,
            calcLines.freshFoodGridLine.gridLinesDegFx100[gridLineIndex]);
      }
      else
      {
         CHECK_EQUAL(temperature,
            calcLines.freezerGridLine.gridLinesDegFx100[gridLineIndex]);
      }
   }

   void WhenTheIceMakerIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, &state);
   }

   void WhenTheCondenserFanAntiSweatBehaviorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledByGrid, &state);
   }

   void TheCondenserFanAntiSweatBehaviorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_CondenserFanAntiSweatBehaviorEnabledByGrid,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheIceMakerIsEnabledAndTheApplicationIsInitialized()
   {
      DataModel_Write(dataModel, Erd_IceMaker0EnableRequest, set);
      GivenApplicationHasBeenInitialized();
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, set);
   }

   void TheIceMakerShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_IceMakerEnabledResolved, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreshFoodAdjustedSetpointShouldBeSetCorrectlyForAdjustedSetpointOf3640DegFx100()
   {
      FreshFoodResolvedSetpointShouldBe(3700);
      FreshFoodCabinetOffsetShouldBe(10);
      FreshFoodCrossAmbientOffsetShouldBe(-100);
      FreshFoodCabinetPlusCrossAmbientOffsetShouldBe(-90);
      FreshFoodSetpointOffsetShouldBe(30);
      FreshFoodHighAmbientOffsetShouldBe(0);
      FreshFoodPulldownOffsetShouldBe(0);
      FreshFoodThermalShiftShouldBe(0);
      FreshFoodAdjustedSetpointShouldBe(3640);
   }

   void FreshFoodAdjustedSetpointShouldBeSetCorrectlyForAdjustedSetpointOf3674DegFx100()
   {
      FreshFoodResolvedSetpointShouldBe(3700);
      FreshFoodCabinetOffsetShouldBe(10);
      FreshFoodCrossAmbientOffsetShouldBe(-50);
      FreshFoodCabinetPlusCrossAmbientOffsetShouldBe(-40);
      FreshFoodSetpointOffsetShouldBe(30);
      FreshFoodHighAmbientOffsetShouldBe(0);
      FreshFoodPulldownOffsetShouldBe(0);
      FreshFoodThermalShiftShouldBe(-16);
      FreshFoodAdjustedSetpointShouldBe(3674);
   }

   void FreezerAdjustedSetpointShouldBeSetCorrectlyForAdjustedSetpointOfNegative40DegFx100()
   {
      FreezerResolvedSetpointShouldBe(0);
      FreezerCabinetOffsetShouldBe(-10);
      FreezerCrossAmbientOffsetShouldBe(-30);
      FreezerCabinetPlusCrossAmbientOffsetShouldBe(-40);
      FreezerHighAmbientOffsetShouldBe(0);
      FreezerThermalShiftShouldBe(0);
      FreezerAdjustedSetpointShouldBe(-40);
   }

   void FreezerAdjustedSetpointShouldBeSetCorrectlyForAdjustedSetpointOfNegative29DegFx100()
   {
      FreezerResolvedSetpointShouldBe(0);
      FreezerCabinetOffsetShouldBe(-10);
      FreezerCrossAmbientOffsetShouldBe(-15);
      FreezerCabinetPlusCrossAmbientOffsetShouldBe(-25);
      FreezerHighAmbientOffsetShouldBe(0);
      FreezerThermalShiftShouldBe(-4);
      FreezerAdjustedSetpointShouldBe(-29);
   }

   void WhenFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatus, &state);
   }

   void WhenFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatus, &state);
   }

   void GivenGridIsInBlock4()
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(4);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);
      WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
      WhenTheIceMakerIs(ENABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(4);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_PullDown);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      TheIceMakerShouldBe(DISABLED);
   }
};

TEST(GridIntegration, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(GridIntegration, ShouldScrollThroughEachGridBlock)
{
   GivenApplicationHasBeenInitialized();
   GivenBothThermistorsAreValid();

   for(uint8_t gridBlockIndex = 0; gridBlockIndex < NumberOfGridBlocks; gridBlockIndex++)
   {
      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks0and1)
{
   for(uint8_t gridBlockIndex = 0; gridBlockIndex <= 1; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);
      WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
      WhenTheIceMakerIs(ENABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_PullDown);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      TheIceMakerShouldBe(DISABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      CompressorRelayShouldBeOnAndFansAtStartupSpeedDuringStartupTime();
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks2and3)
{
   for(uint8_t gridBlockIndex = 2; gridBlockIndex <= 3; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);
      WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
      WhenTheIceMakerIs(ENABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Off);
      TheCoolingSpeedShouldBe(CoolingSpeed_PullDown);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      TheIceMakerShouldBe(DISABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      CompressorRelayShouldBeOnAndFansAtStartupSpeedDuringStartupTimeThenFansShouldTurnOff();
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks4and5and6)
{
   for(uint8_t gridBlockIndex = 4; gridBlockIndex <= 6; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);
      WhenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
      WhenTheIceMakerIs(ENABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_PullDown);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      TheIceMakerShouldBe(DISABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.superHighSpeed);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks7and8and14)
{
   uint8_t gridBlockNumbers[] = { 7, 8, 14 };
   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      WhenTheIceMakerIs(DISABLED);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheIceMakerShouldBe(ENABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks9And10)
{
   for(uint8_t gridBlockIndex = 9; gridBlockIndex <= 10; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
   }

   for(uint8_t gridBlockIndex = 9; gridBlockIndex <= 10; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Off);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks11And12And13)
{
   for(uint8_t gridBlockIndex = 11; gridBlockIndex <= 13; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenCompressorTripMitigationIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheGridAreaShouldBe(GridArea_2);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   }

   for(uint8_t gridBlockIndex = 11; gridBlockIndex <= 13; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);
      WhenCompressorTripMitigationIs(CLEAR);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_2);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);

      WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);
      After(gridData->gridPeriodicRunRateInMSec);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);

      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      After(gridData->gridPeriodicRunRateInMSec);
      TheSingleEvaporatorPulldownActiveShouldBe(SET);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlock15)
{
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(15);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenTheIceMakerIs(DISABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(15);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheIceMakerShouldBe(ENABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
   }

   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(15);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenTheIceMakerIs(DISABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(15);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheIceMakerShouldBe(ENABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreezer);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks16And17)
{
   for(uint8_t gridBlockIndex = 16; gridBlockIndex <= 17; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer);
   }

   for(uint8_t gridBlockIndex = 16; gridBlockIndex <= 17; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks18)
{
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(18);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenCompressorTripMitigationIs(SET);
      WhenBothThermistorsAreValid();

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(18);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_2);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   }

   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(18);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);
      WhenCompressorTripMitigationIs(CLEAR);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(18);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheGridAreaShouldBe(GridArea_2);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks19And20And27And34And41)
{
   uint8_t gridBlockNumbers[] = { 19, 20, 27, 34, 41 };

   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks21)
{
   GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

   WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(21);
   WhenTheCoolingModeIs(CoolingMode_Off);
   WhenTheCoolingSpeedIs(CoolingSpeed_Off);
   WhenTheGridAreaIs(GridArea_2);
   WhenTheSingleEvaporatorPulldownActiveIs(SET);
   WhenTheIceMakerIs(DISABLED);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(21);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   TheIceMakerShouldBe(ENABLED);

   CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

   After(1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks22)
{
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(22);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenTheIceMakerIs(DISABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(22);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheIceMakerShouldBe(ENABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   }

   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(22);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Low);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenTheIceMakerIs(DISABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(22);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheIceMakerShouldBe(ENABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks23And24)
{
   for(uint8_t gridBlockIndex = 23; gridBlockIndex <= 24; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   }

   for(uint8_t gridBlockIndex = 23; gridBlockIndex <= 24; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_Mid);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks25And32And39)
{
   uint8_t gridBlockNumbers[] = { 25, 32, 39 };
   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   }

   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Mid);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks26And33And40)
{
   uint8_t gridBlockNumbers[] = { 26, 33, 40 };
   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   }

   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_High);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks28And29And35)
{
   uint8_t gridBlockNumbers[] = { 28, 29, 35 };

   for(uint8_t index = 0; index < NUM_ELEMENTS(gridBlockNumbers); index++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockNumbers[index]);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenBothThermistorsAreValid();
      WhenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      WhenTheIceMakerIs(DISABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockNumbers[index]);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      TheIceMakerShouldBe(ENABLED);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks30)
{
   GivenApplicationHasBeenInitialized();
   GivenBothThermistorsAreValid();

   WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(30);
   WhenTheCoolingModeIs(CoolingMode_Off);
   WhenTheCoolingSpeedIs(CoolingSpeed_Off);
   WhenTheSingleEvaporatorPulldownActiveIs(SET);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(30);
   TheCoolingModeShouldBe(CoolingMode_Off);
   TheCoolingSpeedShouldBe(CoolingSpeed_Off);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   TheCompressorRelayShouldBe(OFF);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks31)
{
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(31);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(31);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   }

   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(31);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Low);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(31);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks36)
{
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(36);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      WhenTheIceMakerIs(DISABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(36);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      TheIceMakerShouldBe(ENABLED);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   }

   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(36);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      WhenTheIceMakerIs(DISABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(36);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      TheIceMakerShouldBe(ENABLED);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks37)
{
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(37);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(37);
      TheCoolingModeShouldBe(CoolingMode_Off);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   }

   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(37);
      WhenTheCoolingModeIs(CoolingMode_Off);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(37);
      TheCoolingModeShouldBe(CoolingMode_Off);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks38)
{
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(38);
      WhenTheCoolingModeIs(CoolingMode_Freezer);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(38);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   }
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(38);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_Off);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(38);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks42And43)
{
   for(uint8_t gridBlockIndex = 42; gridBlockIndex <= 43; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);
      WhenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      WhenTheIceMakerIs(DISABLED);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      TheIceMakerShouldBe(ENABLED);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks44And45)
{
   for(uint8_t gridBlockIndex = 44; gridBlockIndex <= 45; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_FreshFood);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_Freezer);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_1);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);
   }

   for(uint8_t gridBlockIndex = 44; gridBlockIndex <= 45; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_Freezer);
      WhenTheCoolingSpeedIs(CoolingSpeed_Mid);
      WhenTheGridAreaIs(GridArea_2);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Off);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      TheCompressorRelayShouldBe(OFF);
      TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks46And47)
{
   for(uint8_t gridBlockIndex = 46; gridBlockIndex <= 47; gridBlockIndex++)
   {
      GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

      WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(gridBlockIndex);
      WhenTheCoolingModeIs(CoolingMode_Freezer);
      WhenTheCoolingSpeedIs(CoolingSpeed_High);
      WhenTheGridAreaIs(GridArea_1);
      WhenTheSingleEvaporatorPulldownActiveIs(SET);

      After(gridData->gridPeriodicRunRateInMSec);
      TheGridBlockNumberShouldBe(gridBlockIndex);
      TheCoolingModeShouldBe(CoolingMode_FreshFood);
      TheCoolingSpeedShouldBe(CoolingSpeed_Low);
      TheGridAreaShouldBe(GridArea_2);
      TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
      TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

      CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

      After(1);
      TheCalculatedCondenserFanControlShouldBe(
         condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
      TheCalculatedFreezerEvapFanControlShouldBe(
         freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   }
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks48)
{
   GivenApplicationHasBeenInitialized();
   GivenBothThermistorsAreValid();

   WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(48);
   WhenTheCoolingModeIs(CoolingMode_Off);
   WhenTheCoolingSpeedIs(CoolingSpeed_Off);
   WhenTheGridAreaIs(GridArea_1);
   WhenTheSingleEvaporatorPulldownActiveIs(SET);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(48);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheGridAreaShouldBe(GridArea_2);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   After(1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks48ThenChangeToBlock27LoadsWhenFreezerSensorBecomesInvalid)
{
   GivenApplicationHasBeenInitialized();
   GivenBothThermistorsAreValid();

   WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(48);
   WhenTheCoolingModeIs(CoolingMode_Off);
   WhenTheCoolingSpeedIs(CoolingSpeed_Off);
   WhenTheGridAreaIs(GridArea_1);
   WhenTheSingleEvaporatorPulldownActiveIs(SET);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(48);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheGridAreaShouldBe(GridArea_2);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);

   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   DamperPositionShouldBe(DamperPosition_Closed);
   GridShouldVoteForDamperToBe({ .position = DamperPosition_Open, .care = Vote_Care });

   CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   After(1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreshFood);

   WhenFreezerThermistorValidIs(Invalid);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(27);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_High);
   TheGridAreaShouldBe(GridArea_2);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);

   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(CLEAR);
   DamperPositionShouldBe(DamperPosition_Open);
   GridShouldVoteForDamperToBe({ .position = DamperPosition_Open, .care = Vote_Care });

   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.highSpeedFreshFood);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks38ThenChangeToBlock36LoadsWhenFreshFoodSensorBecomesInvalid)
{
   GivenApplicationHasBeenInitialized();
   GivenBothThermistorsAreValid();

   WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(38);
   WhenTheCoolingModeIs(CoolingMode_Freezer);
   WhenTheCoolingSpeedIs(CoolingSpeed_High);
   WhenTheSingleEvaporatorPulldownActiveIs(SET);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(38);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   After(1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   WhenFreshFoodThermistorValidIs(Invalid);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(36);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   TheCompressorRelayShouldBe(ON);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreezerWithColdSetpoint);
}

TEST(GridIntegration, ShouldControlTheCorrectLoadsForBlocks38ThenTurnAllLoadsOffWhenBothSensorsBecomeInvalid)
{
   GivenApplicationHasBeenInitialized();
   GivenBothThermistorsAreValid();

   WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(38);
   WhenTheCoolingModeIs(CoolingMode_Freezer);
   WhenTheCoolingSpeedIs(CoolingSpeed_High);
   WhenTheSingleEvaporatorPulldownActiveIs(SET);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(38);
   TheCoolingModeShouldBe(CoolingMode_FreshFood);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);

   CompressorRelayAndFansShouldBeOffDuringSabbathDelayAndThenCompressorShouldBeOnAndFanControlsAt(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood,
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   After(1);
   TheCalculatedCondenserFanControlShouldBe(
      condenserFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   WhenFreshFoodThermistorValidIs(Invalid);
   WhenFreezerThermistorValidIs(Invalid);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(36);
   TheCoolingModeShouldBe(CoolingMode_Freezer);
   TheCoolingSpeedShouldBe(CoolingSpeed_Low);
   TheGridAreaShouldBe(GridArea_1);
   TheSingleEvaporatorPulldownActiveShouldBe(CLEAR);
   TheFreshFoodDamperStepperMotorDriveEnableShouldBe(SET);
   TheCalculatedCondenserFanControlShouldBe(fanSpeedOff);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);

   After(compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN - gridData->gridPeriodicRunRateInMSec - 1);
   TheCompressorRelayShouldBe(ON);

   After(1);
   TheCompressorRelayShouldBe(OFF);
}

TEST(GridIntegration, ShouldDisableTheIceMakerWhenTheGridVotesDisabledInBlock0)
{
   GivenReferDataModelTestDoubleIsResetAndApplicationIsInitializedWithValidThermistors();

   WhenFreezerAndFreshFoodTemperaturesAreSetForBlock(0);
   WhenTheCoolingModeIs(CoolingMode_FreshFood);
   WhenTheCoolingSpeedIs(CoolingSpeed_Off);
   WhenTheGridAreaIs(GridArea_2);
   WhenTheSingleEvaporatorPulldownActiveIs(CLEAR);

   After(gridData->gridPeriodicRunRateInMSec);
   WhenTheIceMakerIs(ENABLED);

   After(gridData->gridPeriodicRunRateInMSec);
   TheGridBlockNumberShouldBe(0);
   TheIceMakerShouldBe(DISABLED);
}

TEST(GridIntegration, ShouldCalculateFreshFoodGridLinesWithCrossAmbientHysteresisAdjustmentIncluded)
{
   GivenTheRawSetpointsAre(37, 0);
   GivenTheOffsetsAre(10, 43);
   GivenTheShiftsAre(0, 0);
   GivenTheAmbientTemperatureIs(8000);
   GivenApplicationHasBeenInitialized();

   AmbientWindowAverageTemperatureShouldBe(8000);
   CrossAmbientHysteresisAdjustmentShouldBe(4);
   FreshFoodAdjustedSetpointShouldBeSetCorrectlyForAdjustedSetpointOf3640DegFx100();

   TheCalculatedGridLineTempShouldBe(
      -2086,
      GridDelta_FreshFood,
      GridLine_Nfl);

   TheCalculatedGridLineTempShouldBe(
      3194,
      GridDelta_FreshFood,
      GridLine_FreshFoodLowHyst);

   TheCalculatedGridLineTempShouldBe(
      3494,
      GridDelta_FreshFood,
      GridLine_FreshFoodSetpointDelta);

   TheCalculatedGridLineTempShouldBe(
      4094,
      GridDelta_FreshFood,
      GridLine_FreshFoodHighHyst);

   TheCalculatedGridLineTempShouldBe(
      4594,
      GridDelta_FreshFood,
      GridLine_FreshFoodExtraHigh);

   TheCalculatedGridLineTempShouldBe(
      4794,
      GridDelta_FreshFood,
      GridLine_FreshFoodSuperHigh);

   After(AnHourInMillisecondsToAllowAmbientWindowAverageTemperatureToChange);

   WhenTheAmbientTemperatureChangesTo(9000);

   After(AnHourInMillisecondsToAllowAmbientWindowAverageTemperatureToChange);

   AmbientWindowAverageTemperatureShouldBe(8500);
   CrossAmbientHysteresisAdjustmentShouldBe(2);
   FreshFoodAdjustedSetpointShouldBeSetCorrectlyForAdjustedSetpointOf3674DegFx100();

   TheCalculatedGridLineTempShouldBe(
      -2038,
      GridDelta_FreshFood,
      GridLine_Nfl);

   TheCalculatedGridLineTempShouldBe(
      3226,
      GridDelta_FreshFood,
      GridLine_FreshFoodLowHyst);

   TheCalculatedGridLineTempShouldBe(
      3526,
      GridDelta_FreshFood,
      GridLine_FreshFoodSetpointDelta);

   TheCalculatedGridLineTempShouldBe(
      4126,
      GridDelta_FreshFood,
      GridLine_FreshFoodHighHyst);

   TheCalculatedGridLineTempShouldBe(
      4626,
      GridDelta_FreshFood,
      GridLine_FreshFoodExtraHigh);

   TheCalculatedGridLineTempShouldBe(
      4826,
      GridDelta_FreshFood,
      GridLine_FreshFoodSuperHigh);
}

TEST(GridIntegration, ShouldCalculateFreezerGridLines)
{
   GivenTheRawSetpointsAre(37, 0);
   GivenTheOffsetsAre(10, 43);
   GivenTheShiftsAre(0, 0);
   GivenTheAmbientTemperatureIs(8000);
   GivenApplicationHasBeenInitialized();

   FreezerAdjustedSetpointShouldBeSetCorrectlyForAdjustedSetpointOfNegative40DegFx100();

   TheCalculatedGridLineTempShouldBe(
      -290,
      GridDelta_Freezer,
      GridLine_FreezerLowHyst);

   TheCalculatedGridLineTempShouldBe(
      -190,
      GridDelta_Freezer,
      GridLine_FreezerSetpointDelta);

   TheCalculatedGridLineTempShouldBe(
      -90,
      GridDelta_Freezer,
      GridLine_FreezerHighHyst);

   TheCalculatedGridLineTempShouldBe(
      560,
      GridDelta_Freezer,
      GridLine_FreezerExtraHigh);

   TheCalculatedGridLineTempShouldBe(
      710,
      GridDelta_Freezer,
      GridLine_FreezerSuperHigh);

   TheCalculatedGridLineTempShouldBe(
      5460,
      GridDelta_Freezer,
      GridLine_FreezerExtremeHigh);

   WhenTheAmbientTemperatureChangesTo(9000);

   After(AnHourInMillisecondsToAllowAmbientWindowAverageTemperatureToChange);

   AmbientWindowAverageTemperatureShouldBe(8500);
   CrossAmbientHysteresisAdjustmentShouldBe(2);

   FreezerAdjustedSetpointShouldBeSetCorrectlyForAdjustedSetpointOfNegative29DegFx100();

   TheCalculatedGridLineTempShouldBe(
      -279,
      GridDelta_Freezer,
      GridLine_FreezerLowHyst);

   TheCalculatedGridLineTempShouldBe(
      -179,
      GridDelta_Freezer,
      GridLine_FreezerSetpointDelta);

   TheCalculatedGridLineTempShouldBe(
      -79,
      GridDelta_Freezer,
      GridLine_FreezerHighHyst);

   TheCalculatedGridLineTempShouldBe(
      571,
      GridDelta_Freezer,
      GridLine_FreezerExtraHigh);

   TheCalculatedGridLineTempShouldBe(
      721,
      GridDelta_Freezer,
      GridLine_FreezerSuperHigh);

   TheCalculatedGridLineTempShouldBe(
      5475,
      GridDelta_Freezer,
      GridLine_FreezerExtremeHigh);
}

TEST(GridIntegration, ShouldTurnOffTheFreezerEvapFanWhenDoorIsOpenForTimeoutThenReturnToPreviousSpeed)
{
   GivenGridIsInBlock4();

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   WhenFreezerDoorIs(Open);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);

   After(loadOffDoorOpenData->freezerCompartmentData.timeoutInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);

   After(1);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
}

TEST(GridIntegration, ShouldDelayTurningFanBackOnAfterDoorIsClosed)
{
   GivenGridIsInBlock4();

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   WhenFreezerDoorIs(Open);
   After(loadOffDoorOpenData->freezerCompartmentData.timeoutInSeconds * MSEC_PER_SEC - 1);

   WhenFreezerDoorIs(Closed);

   After(loadOffDoorOpenData->freezerCompartmentData.restartDelayInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);

   After(1);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
}

TEST(GridIntegration, ShouldRestartFanOffTimeoutWhenADoorOpensAfterAllDoorsAreClosed)
{
   GivenGridIsInBlock4();

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);

   WhenFreezerDoorIs(Open);
   After(loadOffDoorOpenData->freezerCompartmentData.timeoutInSeconds * MSEC_PER_SEC - 1);

   WhenFreezerDoorIs(Closed);
   After(loadOffDoorOpenData->freezerCompartmentData.restartDelayInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);

   WhenFreezerDoorIs(Open);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);

   After(loadOffDoorOpenData->freezerCompartmentData.timeoutInSeconds * MSEC_PER_SEC - 1);
   TheCalculatedFreezerEvapFanControlShouldBe(fanSpeedOff);

   After(1);
   TheCalculatedFreezerEvapFanControlShouldBe(
      freezerEvapFanSpeedData->careAboutSetpointData.setpointSpeeds.lowSpeedFreshFood);
}
