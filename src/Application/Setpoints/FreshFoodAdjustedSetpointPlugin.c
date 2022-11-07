/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodAdjustedSetpointPlugin.h"
#include "PersonalityParametricData.h"
#include "SetpointZone.h"
#include "SystemErds.h"

static const SetpointZone_t coldZone = SetpointZone_Cold;
static const SetpointZone_t middleZone = SetpointZone_Middle;
static const SetpointZone_t warmZone = SetpointZone_Warm;
static TemperatureDegFx100_t coldSetpointOffsetInDegFx100;
static TemperatureDegFx100_t middleSetpointOffsetInDegFx100 = 0;
static TemperatureDegFx100_t warmSetpointOffsetInDegFx100;

static const Erd_t freshFoodAdjustedSetpointErds[] = {
   Erd_FreshFood_ResolvedSetpointInDegFx100,
   Erd_FreshFood_CabinetOffsetInDegFx100,
   Erd_FreshFood_SetpointOffsetInDegFx100,
   Erd_FreshFood_CrossAmbientOffsetInDegFx100,
   Erd_FreshFood_PulldownOffsetInDegFx100,
   Erd_FreshFood_ThermalShiftInDegFx100
};

static const I16ErdAdderConfiguration_t freshFoodErdAdderConfig = {
   .resultErd = Erd_FreshFood_AdjustedSetpointInDegFx100,
   .i16ErdsToBeAdded = {
      freshFoodAdjustedSetpointErds,
      NUM_ELEMENTS(freshFoodAdjustedSetpointErds),
   },
};

static ErdWriterOnCompareMatchConfigurationEntry_t erdWriteOnCompareMatchEntries[] = {
   {
      .erdToCompare = Erd_FreshFoodSetpointZone,
      .erdToWrite = Erd_FreshFood_SetpointOffsetInDegFx100,
      .valueToCompare = &coldZone,
      .valueToWrite = &coldSetpointOffsetInDegFx100,
   },
   {
      .erdToCompare = Erd_FreshFoodSetpointZone,
      .erdToWrite = Erd_FreshFood_SetpointOffsetInDegFx100,
      .valueToCompare = &middleZone,
      .valueToWrite = &middleSetpointOffsetInDegFx100,
   },
   {
      .erdToCompare = Erd_FreshFoodSetpointZone,
      .erdToWrite = Erd_FreshFood_SetpointOffsetInDegFx100,
      .valueToCompare = &warmZone,
      .valueToWrite = &warmSetpointOffsetInDegFx100,
   }
};

static const ErdWriterOnCompareMatchConfiguration_t erdWriteOnCompareMatchConfiguration = {
   erdWriteOnCompareMatchEntries,
   NUM_ELEMENTS(erdWriteOnCompareMatchEntries)
};

static void InitializeSetpointOffsetErd(FreshFoodAdjustedSetpointPlugin_t *instance, I_DataModel_t *dataModel)
{
   coldSetpointOffsetInDegFx100 = PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freshFoodAdjustedSetpointData->setpointOffsetData->coldOffsetInDegFx100;
   warmSetpointOffsetInDegFx100 = PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freshFoodAdjustedSetpointData->setpointOffsetData->warmOffsetInDegFx100;
   ErdWriterOnCompareMatch_Init(
      &instance->_private.erdWriterOnCompareMatchForAdjustedSetpoint,
      DataModel_AsDataSource(dataModel),
      &erdWriteOnCompareMatchConfiguration);
}

static void InitializeCabinetOffsetErd(I_DataModel_t *dataModel)
{
   DataModel_Write(
      dataModel,
      Erd_FreshFood_CabinetOffsetInDegFx100,
      &PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freshFoodAdjustedSetpointData->cabinetOffsetInDegFx100);
}

void FreshFoodAdjustedSetpointPlugin_Init(
   FreshFoodAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   InitializeSetpointOffsetErd(instance, dataModel);
   InitializeCabinetOffsetErd(dataModel);
   I16ErdAdder_Init(&instance->_private.freshFoodErdAdder, dataModel, &freshFoodErdAdderConfig);
   FreshFoodShiftOffsetCalculatorPlugin_Init(&instance->_private.freshFoodShiftOffsetCalculatorPlugin, dataModel);
}
