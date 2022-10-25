/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AdjustedSetpointPlugin.h"
#include "Constants_Binary.h"
#include "I16ErdAdder.h"
#include "PersonalityParametricData.h"
#include "SystemErds.h"
#include "utils.h"

// (name, resultErd)
#define ERD_ADDER_TABLE(ENTRY)                                                          \
   ENTRY(freshFood, Erd_FreshFood_AdjustedSetpointInDegFx100)                           \
   ENTRY(freezer, Erd_Freezer_AdjustedSetpointInDegFx100)                               \
   ENTRY(convertibleCompartment, Erd_ConvertibleCompartment_AdjustedSetpointInDegFx100) \
   ENTRY(iceBox, Erd_IceBox_AdjustedSetpointInDegFx100)                                 \
   ENTRY(deliPan, Erd_DeliPan_AdjustedSetpointInDegFx100)

#define EXPAND_AS_ADDERS(name, _resultErd)                                                         \
   static const I16ErdAdderConfiguration_t name##Config = {                                        \
      .resultErd = _resultErd,                                                                     \
      .i16ErdsToBeAdded = { name##AdjustedSetpointErds, NUM_ELEMENTS(name##AdjustedSetpointErds) } \
   };                                                                                              \
                                                                                                   \
   static I16ErdAdder_t name##ErdAdder;                                                            \
   I16ErdAdder_Init(&name##ErdAdder, dataModel, &name##Config);

static const Erd_t freshFoodAdjustedSetpointErds[] = {
   Erd_FreshFood_ResolvedSetpointInDegFx100,
   Erd_FreshFood_CabinetOffsetInDegFx100,
   Erd_FreshFood_SetpointOffsetInDegFx100,
   Erd_FreshFood_CrossAmbientOffsetInDegFx100,
   Erd_FreshFood_PulldownOffsetInDegFx100,
   Erd_FreshFood_ThermalShiftInDegFx100
};

static const Erd_t freezerAdjustedSetpointErds[] = {
   Erd_Freezer_ResolvedSetpointInDegFx100,
   Erd_Freezer_CabinetOffsetInDegFx100,
   Erd_Freezer_CrossAmbientOffsetInDegFx100,
   Erd_Freezer_ThermalShiftInDegFx100
};

static const Erd_t convertibleCompartmentAdjustedSetpointErds[] = {
   Erd_ConvertibleCompartment_ResolvedSetpointInDegFx100,
   Erd_ConvertibleCompartment_SetpointOffsetInDegFx100,
   Erd_ConvertibleCompartment_CrossAmbientOffsetInDegFx100,
   Erd_ConvertibleCompartment_ThermalShiftInDegFx100
};

static const Erd_t iceBoxAdjustedSetpointErds[] = {
   Erd_IceBox_ResolvedSetpointInDegFx100,
   Erd_IceBox_CabinetOffsetInDegFx100,
   Erd_IceBox_IceFormationOffsetInDegFx100,
   Erd_IceBox_ThermalShiftInDegFx100
};

static const Erd_t deliPanAdjustedSetpointErds[] = {
   Erd_DeliPan_ResolvedSetpointInDegFx100,
   Erd_DeliPan_SetpointOffsetInDegFx100,
   Erd_DeliPan_ThermalShiftInDegFx100
};

static const SetpointZone_t coldZone = SetpointZone_Cold;
static const SetpointZone_t middleZone = SetpointZone_Middle;
static const SetpointZone_t warmZone = SetpointZone_Warm;
static TemperatureDegFx100_t coldSetpointOffsetInDegFx100;
static TemperatureDegFx100_t middleSetpointOffsetInDegFx100 = 0;
static TemperatureDegFx100_t warmSetpointOffsetInDegFx100;

// clang-format off
static ErdWriterOnCompareMatchConfigurationEntry_t erdWriteOnCompareMatchEntries[] =
{
   {
      .erdToCompare = Erd_FreshFoodSetpointZone,
      .erdToWrite = Erd_FreshFood_SetpointOffsetInDegFx100,
      .valueToCompare = &coldZone,
      .valueToWrite = &coldSetpointOffsetInDegFx100
   },
   {
      .erdToCompare = Erd_FreshFoodSetpointZone,
      .erdToWrite = Erd_FreshFood_SetpointOffsetInDegFx100,
      .valueToCompare = &middleZone,
      .valueToWrite = &middleSetpointOffsetInDegFx100
   },
   {
      .erdToCompare = Erd_FreshFoodSetpointZone,
      .erdToWrite = Erd_FreshFood_SetpointOffsetInDegFx100,
      .valueToCompare = &warmZone,
      .valueToWrite = &warmSetpointOffsetInDegFx100
   }
};
// clang-format on

static const ErdWriterOnCompareMatchConfiguration_t erdWriteOnCompareMatchConfiguration = {
   erdWriteOnCompareMatchEntries,
   NUM_ELEMENTS(erdWriteOnCompareMatchEntries)
};

static void InitializeCabinetOffsetErds(AdjustedSetpointPlugin_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      Erd_FreshFood_CabinetOffsetInDegFx100,
      &instance->_private.adjustedSetpointData->freshFoodAdjustedSetpointData->cabinetOffsetInDegFx100);

   DataModel_Write(
      instance->_private.dataModel,
      Erd_Freezer_CabinetOffsetInDegFx100,
      &instance->_private.adjustedSetpointData->freezerAdjustedSetpointData->cabinetOffsetInDegFx100);

   DataModel_Write(
      instance->_private.dataModel,
      Erd_IceBox_CabinetOffsetInDegFx100,
      &instance->_private.adjustedSetpointData->iceBoxAdjustedSetpointData->cabinetOffsetInDegFx100);
}

static void InitializeSetpointOffsetErds(AdjustedSetpointPlugin_t *instance)
{
   coldSetpointOffsetInDegFx100 = instance->_private.adjustedSetpointData->freshFoodAdjustedSetpointData->setpointOffsetData->coldOffsetInDegFx100;
   warmSetpointOffsetInDegFx100 = instance->_private.adjustedSetpointData->freshFoodAdjustedSetpointData->setpointOffsetData->warmOffsetInDegFx100;
   ErdWriterOnCompareMatch_Init(
      &instance->_private.erdWriterOnCompareMatchForAdjustedSetpoint,
      DataModel_AsDataSource(instance->_private.dataModel),
      &erdWriteOnCompareMatchConfiguration);
}

void AdjustedSetpointPlugin_Init(AdjustedSetpointPlugin_t *instance, I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.adjustedSetpointData =
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData;

   InitializeCabinetOffsetErds(instance);
   InitializeSetpointOffsetErds(instance);

   ERD_ADDER_TABLE(EXPAND_AS_ADDERS);

   DataModel_Write(dataModel, Erd_AdjustedSetpointPluginReady, set);
}
