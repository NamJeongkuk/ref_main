/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetPlugin.h"
#include "ParametricData.h"
#include "SystemErds.h"

static const FreezerSetpointToIceCabinetSetpointMapperConfig_t freezerSetpointToIceCabinetSetpointMapperConfig = {
   .freezerResolvedSetpoint = Erd_Freezer_ResolvedSetpointInDegFx100,
   .iceCabinetSetpoint = Erd_IceCabinetSetpoint
};

static const IceCabinetGammaCalculatorConfig_t gammaCalculatorConfig = {
   .freezerFilteredTemperatureResolvedInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .iceCabinetTemperatureResolvedInDegFx100Erd = Erd_IceCabinet_FilteredTemperatureResolvedInDegFx100,
   .freezerAdjustedSetpointErd = Erd_Freezer_AdjustedSetpointInDegFx100,
   .iceCabinetAdjustedSetpointErd = Erd_IceCabinet_AdjustedSetpointInDegFx100,
   .gammaErd = Erd_IceCabinetGamma
};

void IceCabinetPlugin_Init(
   IceCabinetPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FreezerSetpointToIceCabinetSetpointMapper_Init(
      &instance->freezerSetpointToIceCabinetSetpointMapper,
      dataModel,
      &freezerSetpointToIceCabinetSetpointMapperConfig,
      PersonalityParametricData_Get(dataModel)->iceCabinetData->freezerSetpointToIceCabinetSetpointTable);

   IceCabinetGammaCalculator_Init(
      &instance->gammaCalculator,
      dataModel,
      &gammaCalculatorConfig,
      &PersonalityParametricData_Get(dataModel)->freshFoodAndFreezerGridData->deltaGridLines->gridLines[GridDelta_SecondDimension],
      &PersonalityParametricData_Get(dataModel)->iceCabinetGridData->deltaGridLines->gridLines[GridDelta_FirstDimension]);

   IceCabinetGridPlugin_Init(
      &instance->iceCabinetGridPlugin,
      dataModel);
}
