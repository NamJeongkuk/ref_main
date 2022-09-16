/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AdjustedSetpointPlugin.h"
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

   ERD_ADDER_TABLE(EXPAND_AS_ADDERS);
}
