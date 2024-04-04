/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Constants_Binary.h"
#include "SystemErds.h"
#include "FreezerFilteredTemperatureTooWarmOnPowerUp.h"
#include "utils.h"

static bool FreezerThermistorIsValid(I_DataModel_t *dataModel)
{
   bool state;
   DataModel_Read(
      dataModel,
      Erd_FreezerThermistor_IsValidResolved,
      &state);

   return state;
}

static void SetErdIfFreezerFilteredTemperatureIsTooWarm(I_DataModel_t *dataModel)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   bool freezerTemperatureIsTooWarmState = false;

   if(FreezerThermistorIsValid(dataModel))
   {
      TemperatureDegFx100_t freezerFilteredResolvedTemperatureInDegFx100;
      DataModel_Read(
         dataModel,
         Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
         &freezerFilteredResolvedTemperatureInDegFx100);

      TwoDimensionalCalculatedGridLines_t calcGridLines;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,
         &calcGridLines);

      TemperatureDegFx100_t gridFreezerExtremeHystTemperature = calcGridLines.secondDimensionGridLines.gridLinesDegFx100[GridLine_FreezerExtremeHigh];

      freezerTemperatureIsTooWarmState = ((freezerFilteredResolvedTemperatureInDegFx100 > gridFreezerExtremeHystTemperature) ||
         (freezerFilteredResolvedTemperatureInDegFx100 >= defrostData->heaterOnData.freezerDefrostTerminationTemperatureInDegFx100));
   }

   DataModel_Write(
      dataModel,
      Erd_FreezerFilteredTemperatureTooWarmOnPowerUp,
      &freezerTemperatureIsTooWarmState);
}

void FreezerFilteredTemperatureTooWarmOnPowerUp_Init(I_DataModel_t *dataModel)
{
   SetErdIfFreezerFilteredTemperatureIsTooWarm(dataModel);

   DataModel_Write(
      dataModel,
      Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
      set);
}
