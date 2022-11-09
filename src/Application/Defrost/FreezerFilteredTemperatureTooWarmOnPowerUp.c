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

static void SetErdIfFreezerFilteredTemperatureIsTooWarm(I_DataModel_t *dataModel)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(dataModel)->defrostData;

   TemperatureDegFx100_t freezerFilteredResolvedTemperatureInDegFx100;
   DataModel_Read(
      dataModel,
      Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
      &freezerFilteredResolvedTemperatureInDegFx100);

   CalculatedGridLines_t calcGridLines;
   DataModel_Read(
      dataModel,
      Erd_Grid_CalculatedGridLines,
      &calcGridLines);

   TemperatureDegFx100_t gridFreezerExtremeHystTemperature = calcGridLines.freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh];

   bool state = ((freezerFilteredResolvedTemperatureInDegFx100 > gridFreezerExtremeHystTemperature) ||
      (freezerFilteredResolvedTemperatureInDegFx100 >= defrostData->freezerDefrostTerminationTemperatureInDegFx100));

   DataModel_Write(
      dataModel,
      Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
      &state);
}

void FreezerFilteredTemperatureTooWarmOnPowerUp_Init(I_DataModel_t *dataModel)
{
   SetErdIfFreezerFilteredTemperatureIsTooWarm(dataModel);

   DataModel_Write(
      dataModel,
      Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
      set);
}
