/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostParameterSelector.h"
#include "PersonalityParametricData.h"
#include "DefrostData.h"
#include "EvaporatorData.h"
#include "SystemErds.h"
#include "EnhancedSabbathData.h"
#include "Constants_Binary.h"

static void SetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostWhenEnhancedSabbathModeIs(DefrostParameterSelector_t *instance, bool enhancedSabbathMode)
{
   uint8_t numberOfFreshFoodDefrostsBeforeAFreezerDefrost;

   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;
   const EnhancedSabbathData_t *enhancedSabbathData = PersonalityParametricData_Get(instance->_private.dataModel)->enhancedSabbathData;

   if(enhancedSabbathMode)
   {
      numberOfFreshFoodDefrostsBeforeAFreezerDefrost = enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   }
   else
   {
      numberOfFreshFoodDefrostsBeforeAFreezerDefrost = defrostData->numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   }

   DataModel_Write(
      instance->_private.dataModel,
      Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
      &numberOfFreshFoodDefrostsBeforeAFreezerDefrost);
}

static void SetMaxPrechillTimeInMinutes(DefrostParameterSelector_t *instance, bool defrostIsFreshFoodOnly)
{
   uint8_t maxPrechillTimeInMinutes;

   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;

   if(defrostIsFreshFoodOnly)
   {
      maxPrechillTimeInMinutes = defrostData->maxPrechillTimeForFreshFoodOnlyDefrostInMinutes;
   }
   else
   {
      maxPrechillTimeInMinutes = defrostData->maxPrechillTimeInMinutes;
   }

   DataModel_Write(
      instance->_private.dataModel,
      Erd_MaxPrechillTimeInMinutes,
      &maxPrechillTimeInMinutes);
}

static void SetPostDwellExitTimeInMinutes(DefrostParameterSelector_t *instance, bool defrostIsFreshFoodOnly)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;
   const EvaporatorData_t *evaporatorData = PersonalityParametricData_Get(instance->_private.dataModel)->evaporatorData;

   uint8_t postDwellExitTimeInMinutes = 0;

   if(evaporatorData->numberOfEvaporators == 1)
   {
      postDwellExitTimeInMinutes = defrostData->postDwellExitTimeInMinutes;
   }
   else
   {
      if(defrostIsFreshFoodOnly)
      {
         postDwellExitTimeInMinutes = defrostData->freshFoodOnlyPostDwellExitTimeInMinutes;
      }
      else
      {
         postDwellExitTimeInMinutes = defrostData->postDwellExitTimeInMinutes;
      }
   }

   DataModel_Write(
      instance->_private.dataModel,
      Erd_PostDwellExitTimeInMinutes,
      &postDwellExitTimeInMinutes);
}

static void SetTimeWhenDefrostReadyTimerIsSatisfiedInMinutes(DefrostParameterSelector_t *instance)
{
   bool freshFoodDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_FreshFoodDefrostWasAbnormal,
      &freshFoodDefrostWasAbnormal);

   bool freezerDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_FreezerDefrostWasAbnormal,
      &freezerDefrostWasAbnormal);

   bool convertibleCompartmentDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_ConvertibleCompartmentDefrostWasAbnormal,
      &convertibleCompartmentDefrostWasAbnormal);

   bool freezerEvaporatorThermistorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_FreezerEvaporatorThermistorIsValid,
      &freezerEvaporatorThermistorIsValid);

   if(freshFoodDefrostWasAbnormal ||
      freezerDefrostWasAbnormal ||
      convertibleCompartmentDefrostWasAbnormal ||
      !freezerEvaporatorThermistorIsValid)
   {
      const DefrostData_t *defrostData = PersonalityParametricData_Get(instance->_private.dataModel)->defrostData;

      DataModel_Write(
         instance->_private.dataModel,
         Erd_TimeInMinutesUntilReadyToDefrost,
         &defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   }
   else
   {
      uint16_t maxTimeBetweenDefrostsInMinutes;
      DataModel_Read(
         instance->_private.dataModel,
         Erd_MaxTimeBetweenDefrostsInMinutes,
         &maxTimeBetweenDefrostsInMinutes);

      DataModel_Write(
         instance->_private.dataModel,
         Erd_TimeInMinutesUntilReadyToDefrost,
         &maxTimeBetweenDefrostsInMinutes);
   }
}

static void DataModelChanged(void *context, const void *args)
{
   DefrostParameterSelector_t *instance = context;

   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == Erd_DefrostIsFreshFoodOnly)
   {
      const bool *defrostIsFreshFoodOnly = onChangeData->data;
      SetMaxPrechillTimeInMinutes(instance, *defrostIsFreshFoodOnly);
      SetPostDwellExitTimeInMinutes(instance, *defrostIsFreshFoodOnly);
   }
   else if(erd == Erd_EnhancedSabbathMode)
   {
      const bool *enhancedSabbathMode = onChangeData->data;
      SetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostWhenEnhancedSabbathModeIs(instance, *enhancedSabbathMode);
   }
   else if(erd == Erd_FreshFoodDefrostWasAbnormal ||
      erd == Erd_FreezerDefrostWasAbnormal ||
      erd == Erd_ConvertibleCompartmentDefrostWasAbnormal ||
      erd == Erd_MaxTimeBetweenDefrostsInMinutes ||
      erd == Erd_FreezerEvaporatorThermistorIsValid)
   {
      SetTimeWhenDefrostReadyTimerIsSatisfiedInMinutes(instance);
   }
}

void DefrostParameterSelector_Init(DefrostParameterSelector_t *instance, I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;

   bool defrostIsFreshFoodOnly;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_DefrostIsFreshFoodOnly,
      &defrostIsFreshFoodOnly);

   bool enhancedSabbathMode;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_EnhancedSabbathMode,
      &enhancedSabbathMode);

   SetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostWhenEnhancedSabbathModeIs(instance, enhancedSabbathMode);
   SetMaxPrechillTimeInMinutes(instance, defrostIsFreshFoodOnly);
   SetPostDwellExitTimeInMinutes(instance, defrostIsFreshFoodOnly);
   SetTimeWhenDefrostReadyTimerIsSatisfiedInMinutes(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);

   DataModel_Write(
      instance->_private.dataModel,
      Erd_DefrostParameterSelectorReady,
      set);
}
