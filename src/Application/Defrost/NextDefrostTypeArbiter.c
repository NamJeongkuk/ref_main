/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "NextDefrostTypeArbiter.h"
#include "Constants_Binary.h"
#include "DefrostType.h"
#include "ConvertibleCompartmentStateType.h"
#include "PersonalityParametricData.h"
#include "utils.h"

static void UpdateNextDefrostTypeTo(
   NextDefrostTypeArbiter_t *instance,
   DefrostType_t defrostType)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->nextDefrostTypeErd,
      &defrostType);
}

static void UpdateCurrentDefrostTypeTo(
   NextDefrostTypeArbiter_t *instance,
   DefrostType_t defrostType)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->currentDefrostTypeErd,
      &defrostType);
}

static DefrostType_t CurrentDefrostType(NextDefrostTypeArbiter_t *instance)
{
   DefrostType_t currentDefrostType;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->currentDefrostTypeErd,
      &currentDefrostType);

   return currentDefrostType;
}

static void IncreaseFreshFoodDefrostCount(NextDefrostTypeArbiter_t *instance)
{
   uint8_t freshFoodDefrostCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostCountErd,
      &freshFoodDefrostCount);

   freshFoodDefrostCount++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostCountErd,
      &freshFoodDefrostCount);
}

static bool EnhancedSabbathModeIsEnabled(NextDefrostTypeArbiter_t *instance)
{
   bool enhancedSabbathModeIsEnabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathModeErd,
      &enhancedSabbathModeIsEnabled);

   return enhancedSabbathModeIsEnabled;
}

static bool FreezerDefrostWasAbnormal(NextDefrostTypeArbiter_t *instance)
{
   bool freezerDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
      &freezerDefrostWasAbnormal);

   return freezerDefrostWasAbnormal;
}

static bool ConvertibleCompartmentIsActingAsFreezerAndDefrostWasAbnormal(NextDefrostTypeArbiter_t *instance)
{
   bool hasConvertibleCompartment;
   bool convertibleCompartmentDefrostWasAbnormal;
   ConvertibleCompartmentStateType_t convertibleCompartmentStateType;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->hasConvertibleCompartmentErd,
      &hasConvertibleCompartment);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd,
      &convertibleCompartmentDefrostWasAbnormal);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentStateErd,
      &convertibleCompartmentStateType);

   return (hasConvertibleCompartment &&
      convertibleCompartmentDefrostWasAbnormal &&
      (convertibleCompartmentStateType == ConvertibleCompartmentStateType_Freezer));
}

static void DetermineNumberOfFreshFoodDefrostsBeforeAFreezerDefrost(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfFreshFoodDefrostsBeforeAFreezerDefrost;

   if(EnhancedSabbathModeIsEnabled(instance))
   {
      numberOfFreshFoodDefrostsBeforeAFreezerDefrost =
         instance->_private.enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   }
   else if(FreezerDefrostWasAbnormal(instance) || (ConvertibleCompartmentIsActingAsFreezerAndDefrostWasAbnormal(instance)))
   {
      numberOfFreshFoodDefrostsBeforeAFreezerDefrost =
         instance->_private.defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost;
   }
   else
   {
      numberOfFreshFoodDefrostsBeforeAFreezerDefrost =
         instance->_private.defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd,
      &numberOfFreshFoodDefrostsBeforeAFreezerDefrost);
}

static uint8_t FreshFoodDefrostCount(NextDefrostTypeArbiter_t *instance)
{
   uint8_t freshFoodDefrostCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostCountErd,
      &freshFoodDefrostCount);

   return freshFoodDefrostCount;
}

static uint8_t NumberOfFreshFoodDefrostsBeforeAFreezerDefrost(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfFreshFoodDefrosts;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd,
      &numberOfFreshFoodDefrosts);

   return numberOfFreshFoodDefrosts;
}

static void ResetFreshFoodDefrostCount(NextDefrostTypeArbiter_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostCountErd,
      clear);
}

static bool FreezerFilteredTemperatureTooWarmAtPowerUpIsSet(NextDefrostTypeArbiter_t *instance)
{
   bool freezerFilteredTemperatureTooWarmAtPowerUpIsSet;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureTooWarmAtPowerUpErd,
      &freezerFilteredTemperatureTooWarmAtPowerUpIsSet);

   return freezerFilteredTemperatureTooWarmAtPowerUpIsSet;
}

static void DataModelUpdated(void *context, const void *_args)
{
   NextDefrostTypeArbiter_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->defrostingErd)
   {
      const bool *defrosting = args->data;
      if(*defrosting)
      {
         DefrostType_t currentDefrostType = CurrentDefrostType(instance);
         if(currentDefrostType == DefrostType_FreshFood)
         {
            IncreaseFreshFoodDefrostCount(instance);

            if(FreshFoodDefrostCount(instance) >= NumberOfFreshFoodDefrostsBeforeAFreezerDefrost(instance))
            {
               UpdateNextDefrostTypeTo(instance, DefrostType_Full);
            }
         }
         else if(currentDefrostType == DefrostType_Full)
         {
            ResetFreshFoodDefrostCount(instance);
            UpdateNextDefrostTypeTo(instance, DefrostType_FreshFood);
         }
      }
   }
   else if((args->erd == instance->_private.config->enhancedSabbathModeErd) ||
      (args->erd == instance->_private.config->freezerDefrostWasAbnormalErd) ||
      (args->erd == instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd) ||
      (args->erd == instance->_private.config->convertibleCompartmentStateErd))
   {
      DetermineNumberOfFreshFoodDefrostsBeforeAFreezerDefrost(instance);
   }
}

void NextDefrostTypeArbiter_Init(
   NextDefrostTypeArbiter_t *instance,
   I_DataModel_t *dataModel,
   const NextDefrostTypeArbiterConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.enhancedSabbathData = PersonalityParametricData_Get(dataModel)->enhancedSabbathData;

   DetermineNumberOfFreshFoodDefrostsBeforeAFreezerDefrost(instance);

   if(NumberOfFreshFoodDefrostsBeforeAFreezerDefrost(instance) == 0)
   {
      UpdateCurrentDefrostTypeTo(instance, DefrostType_Full);
      UpdateNextDefrostTypeTo(instance, DefrostType_Full);
   }
   else
   {
      if(FreezerFilteredTemperatureTooWarmAtPowerUpIsSet(instance))
      {
         UpdateCurrentDefrostTypeTo(instance, DefrostType_Full);
         UpdateNextDefrostTypeTo(instance, DefrostType_Full);
      }

      EventSubscription_Init(
         &instance->_private.dataModelSubscription,
         instance,
         DataModelUpdated);

      DataModel_SubscribeAll(
         dataModel,
         &instance->_private.dataModelSubscription);
   }
}
