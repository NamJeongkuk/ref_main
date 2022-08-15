/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodOnlyDefrostArbitrator.h"
#include "PersonalityParametricData.h"
#include "utils.h"
#include "uassert.h"

static void UpdateFreshFoodOnlyDefrostErd(FreshFoodOnlyDefrostArbitrator_t *instance)
{
   uint8_t numberOfFreshFoodDefrostsBeforeAFreezerDefrost;
   bool freezerDefrostWasAbnormal;
   bool freshFoodOnlyDefrost = true;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd,
      &numberOfFreshFoodDefrostsBeforeAFreezerDefrost);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->freezerDefrostWasAbnormalErd,
      &freezerDefrostWasAbnormal);

   if((freezerDefrostWasAbnormal &&
         numberOfFreshFoodDefrostsBeforeAFreezerDefrost >
            instance->_private.defrostParametricData->numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost) ||
      (numberOfFreshFoodDefrostsBeforeAFreezerDefrost >=
         instance->_private.defrostParametricData->numberOfFreshFoodDefrostsBeforeFreezerDefrost))
   {
      freshFoodOnlyDefrost = false;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->defrostIsFreshFoodOnlyErd,
      &freshFoodOnlyDefrost);
}

static void DataModelChanged(void *context, const void *_args)
{
   REINTERPRET(instance, context, FreshFoodOnlyDefrostArbitrator_t *);
   REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);

   if(args->erd == instance->_private.configuration->numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd ||
      args->erd == instance->_private.configuration->freezerDefrostWasAbnormalErd)
   {
      UpdateFreshFoodOnlyDefrostErd(instance);
   }
}

void FreshFoodOnlyDefrostArbitrator_Init(
   FreshFoodOnlyDefrostArbitrator_t *instance,
   I_DataModel_t *dataModel,
   const FreshFoodOnlyDefrostArbitratorConfiguration_t *configuration)
{
   bool defrostParameterSelectorReady;
   DataModel_Read(
      dataModel,
      configuration->defrostParameterSelectorReadyErd,
      &defrostParameterSelectorReady);

   uassert(defrostParameterSelectorReady);

   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;

   EventSubscription_Init(
      &instance->_private.onDataModelChange,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.onDataModelChange);

   UpdateFreshFoodOnlyDefrostErd(instance);
}
