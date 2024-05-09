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
#include "uassert.h"

static const SetpointZone_t coldZone = SetpointZone_Cold;
static const SetpointZone_t middleZone = SetpointZone_Middle;
static const SetpointZone_t warmZone = SetpointZone_Warm;
static TemperatureDegFx100_t coldSetpointOffsetInDegFx100;
static TemperatureDegFx100_t middleSetpointOffsetInDegFx100 = 0;
static TemperatureDegFx100_t warmSetpointOffsetInDegFx100;

static const Erd_t freshFoodAdjustedSetpointWithoutShiftErds[] = {
   Erd_FreshFood_ResolvedSetpointInDegFx100,
   Erd_FreshFood_CabinetOffsetInDegFx100,
   Erd_FreshFood_SetpointOffsetInDegFx100,
   Erd_FreshFood_CrossAmbientOffsetInDegFx100,
   Erd_FreshFood_HighAmbientOffsetInDegFx100,
   Erd_FreshFood_PulldownOffsetInDegFx100,
};

static const I16ErdAdderConfiguration_t freshFoodAdjustedSetpointWithoutShiftConfig = {
   .resultErd = Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100,
   .i16ErdsToBeAdded = {
      freshFoodAdjustedSetpointWithoutShiftErds,
      NUM_ELEMENTS(freshFoodAdjustedSetpointWithoutShiftErds),
   },
};

static const Erd_t freshFoodAdjustedSetpointErds[] = {
   Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100,
   Erd_FreshFood_ThermalShiftInDegFx100,
};

static const I16ErdAdderConfiguration_t freshFoodAdjustedSetpointConfig = {
   .resultErd = Erd_FreshFood_AdjustedSetpointInDegFx100,
   .i16ErdsToBeAdded = {
      freshFoodAdjustedSetpointErds,
      NUM_ELEMENTS(freshFoodAdjustedSetpointErds),
   },
};

static const ErdWriterOnCompareMatchConfigurationEntry_t erdWriteOnCompareMatchEntries[] = {
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

static const ResolvedSetpointWriterConfiguration_t freshFoodResolvedSetpointWriterConfiguration = {
   .setpointVoteErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .resolvedSetpointErd = Erd_FreshFood_ResolvedSetpointInDegFx100,
   .userSetpointPluginReadyErd = Erd_UserSetpointPluginReady
};

static const CrossAmbientOffsetCalculatorConfig_t freshFoodCrossAmbientOffsetCalculatorConfig = {
   .ambientWindowAveragedTemperatureInDegFx100Erd = Erd_Ambient_WindowAveragedTemperatureInDegFx100,
   .crossAmbientOffsetInDegFx100Erd = Erd_FreshFood_CrossAmbientOffsetInDegFx100,
};

static const HighAmbientOffsetCalculatorConfig_t freshFoodHighAmbientOffsetCalculatorConfig = {
   .ambientFilteredHumidityResolvedPercentx100Erd = Erd_Ambient_FilteredHumidityResolvedPercentx100,
   .highAmbientOffsetErd = Erd_FreshFood_HighAmbientOffsetInDegFx100,
   .setpointStatusErd = Erd_FreshFoodSetpointStatus,
   .ambientHumiditySensorIsValidResolvedErd = Erd_AmbientHumidity_IsValidResolved
};

static bool SetpointZonePluginIsReady(I_DataModel_t *dataModel)
{
   bool state;
   DataModel_Read(dataModel, Erd_SetpointZonePluginReady, &state);
   return state;
}

static void InitializeSetpointOffsetErd(FreshFoodAdjustedSetpointPlugin_t *instance, I_DataModel_t *dataModel)
{
   uassert(SetpointZonePluginIsReady(dataModel));

   coldSetpointOffsetInDegFx100 = PersonalityParametricData_Get(dataModel)->freshFoodThermalOffsetData->setpointOffsetData->coldOffsetInDegFx100;
   warmSetpointOffsetInDegFx100 = PersonalityParametricData_Get(dataModel)->freshFoodThermalOffsetData->setpointOffsetData->warmOffsetInDegFx100;

   SetpointZone_t freshFoodSetpointZone;
   DataModel_Read(dataModel, Erd_FreshFoodSetpointZone, &freshFoodSetpointZone);
   switch(freshFoodSetpointZone)
   {
      case SetpointZone_Cold:
         DataModel_Write(dataModel, Erd_FreshFood_SetpointOffsetInDegFx100, &coldSetpointOffsetInDegFx100);
         break;

      case SetpointZone_Middle:
         DataModel_Write(dataModel, Erd_FreshFood_SetpointOffsetInDegFx100, &middleSetpointOffsetInDegFx100);
         break;

      case SetpointZone_Warm:
         DataModel_Write(dataModel, Erd_FreshFood_SetpointOffsetInDegFx100, &warmSetpointOffsetInDegFx100);
         break;

      default:
         break;
   }

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
      &PersonalityParametricData_Get(dataModel)->freshFoodThermalOffsetData->cabinetOffsetInDegFx100);
}

void FreshFoodAdjustedSetpointPlugin_Init(
   FreshFoodAdjustedSetpointPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   InitializeSetpointOffsetErd(instance, dataModel);
   InitializeCabinetOffsetErd(dataModel);
   ResolvedSetpointWriter_Init(
      &instance->_private.freshFoodResolvedSetpointWriter,
      dataModel,
      &freshFoodResolvedSetpointWriterConfiguration);
   I16ErdAdder_Init(&instance->_private.freshFoodAdjustedSetpointWithoutShiftErdAdder, dataModel, &freshFoodAdjustedSetpointWithoutShiftConfig);
   I16ErdAdder_Init(&instance->_private.freshFoodAdjustedSetpointErdAdder, dataModel, &freshFoodAdjustedSetpointConfig);
   CrossAmbientOffsetCalculator_Init(
      &instance->_private.freshFoodCrossAmbientOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->freshFoodThermalOffsetData->crossAmbientOffsetData,
      &freshFoodCrossAmbientOffsetCalculatorConfig);
   HighAmbientHumidityOffsetCalculator_Init(
      &instance->_private.freshFoodHighAmbientOffsetCalculator,
      dataModel,
      PersonalityParametricData_Get(dataModel)->freshFoodThermalOffsetData->highAmbientOffsetData,
      &freshFoodHighAmbientOffsetCalculatorConfig);
   FreshFoodShiftOffsetCalculatorPlugin_Init(&instance->_private.freshFoodShiftOffsetCalculatorPlugin, dataModel);
}
