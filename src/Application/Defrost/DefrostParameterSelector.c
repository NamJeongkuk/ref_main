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
   uint8_t maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes;

   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance.dataModel)->defrostData;

   if(defrostIsFreshFoodOnly)
   {
      maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes = defrostData->maxPrechillTimeForFreshFoodOnlyDefrostInMinutes;
   }
   else
   {
      maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes = defrostData->maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes;
   }

   DataModel_Write(
      instance.dataModel,
      Erd_MaxPrechillTimeInMinutes,
      &maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes);
}

static void SetPostDwellExitTimeInMinutes(bool defrostIsFreshFoodOnly)
{
   const DefrostData_t *defrostData = PersonalityParametricData_Get(instance.dataModel)->defrostData;
   const EvaporatorData_t *evaporatorData = PersonalityParametricData_Get(instance.dataModel)->evaporatorData;

   uint8_t postDwellExitTimeInMinutes = 0;

   if(evaporatorData->numberOfEvaporators == 1)
   {
      postDwellExitTimeInMinutes = defrostData->freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes;
   }
   else
   {
      if(defrostIsFreshFoodOnly)
      {
         postDwellExitTimeInMinutes = defrostData->freshFoodOnlyPostDwellExitTimeInMinutes;
      }
      else
      {
         postDwellExitTimeInMinutes = defrostData->freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes;
      }
   }

   DataModel_Write(
      instance.dataModel,
      Erd_PostDwellExitTimeInMinutes,
      &postDwellExitTimeInMinutes);
}

static void DataModelChanged(void *context, const void *args)
{
   IGNORE(context);

   REINTERPRET(onChangeData, args, const DataModelOnDataChangeArgs_t *);
   REINTERPRET(erd, onChangeData->erd, Erd_t);

   if(erd == Erd_DefrostIsFreshFoodOnly)
   {
      REINTERPRET(defrostIsFreshFoodOnly, onChangeData->data, const bool *);
      SetMaxPrechillTimeInMinutes(*defrostIsFreshFoodOnly);
      SetPostDwellExitTimeInMinutes(*defrostIsFreshFoodOnly);
   }
   else if(erd == Erd_EnhancedSabbathMode)
   {
      REINTERPRET(enhancedSabbathMode, onChangeData->data, const bool *);
      SetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostWhenEnhancedSabbathModeIs(*enhancedSabbathMode);
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

   EventSubscription_Init(
      &instance.dataModelSubscription,
      NULL,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance.dataModelSubscription);
}
