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

static struct
{
   EventSubscription_t dataModelSubscription;
   I_DataModel_t *dataModel;
} instance;

static void SetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostWhenEnhancedSabbathModeIs(bool enhancedSabbathMode)
{
   uint8_t numberOfFreshFoodDefrostsBeforeAFreezerDefrost;

   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance.dataModel)->defrostData;
   const EnhancedSabbathData_t *enhancedSabbathData = PersonalityParametricData_Get(instance.dataModel)->enhancedSabbathData;

   if(enhancedSabbathMode)
   {
      numberOfFreshFoodDefrostsBeforeAFreezerDefrost = enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   }
   else
   {
      numberOfFreshFoodDefrostsBeforeAFreezerDefrost = defrostData->numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   }

   DataModel_Write(
      instance.dataModel,
      Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
      &numberOfFreshFoodDefrostsBeforeAFreezerDefrost);
}

static void SetMaxPrechillTimeInMinutes(bool defrostIsFreshFoodOnly)
{
   uint8_t maxPrechillTimeInMinutes;

   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance.dataModel)->defrostData;

   if(defrostIsFreshFoodOnly)
   {
      maxPrechillTimeInMinutes = defrostData->maxPrechillTimeForFreshFoodOnlyDefrostInMinutes;
   }
   else
   {
      maxPrechillTimeInMinutes = defrostData->maxPrechillTimeInMinutes;
   }

   DataModel_Write(
      instance.dataModel,
      Erd_MaxPrechillTimeInMinutes,
      &maxPrechillTimeInMinutes);
}

static void SetPostDwellExitTimeInMinutes(bool defrostIsFreshFoodOnly)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance.dataModel)->defrostData;
   const EvaporatorData_t *evaporatorData = PersonalityParametricData_Get(instance.dataModel)->evaporatorData;

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
      instance.dataModel,
      Erd_PostDwellExitTimeInMinutes,
      &postDwellExitTimeInMinutes);
}

static void SetTimeWhenDefrostReadyTimerIsSatisfiedInMinutes(void)
{
   bool freshFoodDefrostWasAbnormal;
   DataModel_Read(
      instance.dataModel,
      Erd_FreshFoodDefrostWasAbnormal,
      &freshFoodDefrostWasAbnormal);

   bool freezerDefrostWasAbnormal;
   DataModel_Read(
      instance.dataModel,
      Erd_FreezerDefrostWasAbnormal,
      &freezerDefrostWasAbnormal);

   bool convertibleCompartmentDefrostWasAbnormal;
   DataModel_Read(
      instance.dataModel,
      Erd_ConvertibleCompartmentDefrostWasAbnormal,
      &convertibleCompartmentDefrostWasAbnormal);

   if(freshFoodDefrostWasAbnormal || freezerDefrostWasAbnormal || convertibleCompartmentDefrostWasAbnormal)
   {
      const DefrostData_t *defrostData = PersonalityParametricData_Get(instance.dataModel)->defrostData;

      DataModel_Write(
         instance.dataModel,
         Erd_TimeInMinutesWhenDefrostReadyTimerIsSatisfied,
         &defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
   }
   else
   {
      uint16_t maxTimeBetweenDefrostsInMinutes;
      DataModel_Read(
         instance.dataModel,
         Erd_MaxTimeBetweenDefrostsInMinutes,
         &maxTimeBetweenDefrostsInMinutes);

      DataModel_Write(
         instance.dataModel,
         Erd_TimeInMinutesWhenDefrostReadyTimerIsSatisfied,
         &maxTimeBetweenDefrostsInMinutes);
   }
}

static void DataModelChanged(void *context, const void *args)
{
   IGNORE(context);

   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == Erd_DefrostIsFreshFoodOnly)
   {
      const bool *defrostIsFreshFoodOnly = onChangeData->data;
      SetMaxPrechillTimeInMinutes(*defrostIsFreshFoodOnly);
      SetPostDwellExitTimeInMinutes(*defrostIsFreshFoodOnly);
   }
   else if(erd == Erd_EnhancedSabbathMode)
   {
      const bool *enhancedSabbathMode = onChangeData->data;
      SetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostWhenEnhancedSabbathModeIs(*enhancedSabbathMode);
   }
   else if(erd == Erd_FreshFoodDefrostWasAbnormal ||
      erd == Erd_FreezerDefrostWasAbnormal ||
      erd == Erd_ConvertibleCompartmentDefrostWasAbnormal ||
      erd == Erd_MaxTimeBetweenDefrostsInMinutes)
   {
      SetTimeWhenDefrostReadyTimerIsSatisfiedInMinutes();
   }
}

void DefrostParameterSelector_Init(I_DataModel_t *dataModel)
{
   instance.dataModel = dataModel;

   bool defrostIsFreshFoodOnly;
   DataModel_Read(
      instance.dataModel,
      Erd_DefrostIsFreshFoodOnly,
      &defrostIsFreshFoodOnly);

   bool enhancedSabbathMode;
   DataModel_Read(
      instance.dataModel,
      Erd_EnhancedSabbathMode,
      &enhancedSabbathMode);

   SetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostWhenEnhancedSabbathModeIs(enhancedSabbathMode);
   SetMaxPrechillTimeInMinutes(defrostIsFreshFoodOnly);
   SetPostDwellExitTimeInMinutes(defrostIsFreshFoodOnly);
   SetTimeWhenDefrostReadyTimerIsSatisfiedInMinutes();

   EventSubscription_Init(
      &instance.dataModelSubscription,
      NULL,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance.dataModelSubscription);
}
