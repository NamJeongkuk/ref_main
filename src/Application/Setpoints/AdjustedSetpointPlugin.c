/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AdjustedSetpointPlugin.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"
#include "utils.h"

static void InitializeCabinetOffsetErds(I_DataModel_t *dataModel)
{
   const AdjustedSetpointData_t *adjustedSetpointData =
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData;

   DataModel_Write(
      dataModel,
      Erd_FreshFood_CabinetOffsetInDegFx100,
      &adjustedSetpointData->freshFoodAdjustedSetpointData->cabinetOffsetInDegFx100);

   DataModel_Write(
      dataModel,
      Erd_Freezer_CabinetOffsetInDegFx100,
      &adjustedSetpointData->freezerAdjustedSetpointData->cabinetOffsetInDegFx100);

   DataModel_Write(
      dataModel,
      Erd_IceBox_CabinetOffsetInDegFx100,
      &adjustedSetpointData->iceBoxAdjustedSetpointData->cabinetOffsetInDegFx100);
}

void AdjustedSetpointPlugin_Init(I_DataModel_t *dataModel)
{
   InitializeCabinetOffsetErds(dataModel);
}
