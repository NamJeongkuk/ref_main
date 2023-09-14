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

static void IncrementNumberOfFreshFoodDefrosts(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfFreshFoodDefrosts;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsErd,
      &numberOfFreshFoodDefrosts);

   numberOfFreshFoodDefrosts++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsErd,
      &numberOfFreshFoodDefrosts);
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

static void DetermineNumberOfFreshFoodDefrostsBeforeAFullDefrost(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfFreshFoodDefrostsBeforeAFullDefrost;

   if(EnhancedSabbathModeIsEnabled(instance))
   {
      numberOfFreshFoodDefrostsBeforeAFullDefrost =
         instance->_private.enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   }
   else if(FreezerDefrostWasAbnormal(instance) || (ConvertibleCompartmentIsActingAsFreezerAndDefrostWasAbnormal(instance)))
   {
      numberOfFreshFoodDefrostsBeforeAFullDefrost =
         instance->_private.defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost;
   }
   else
   {
      numberOfFreshFoodDefrostsBeforeAFullDefrost =
         instance->_private.defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsBeforeAFullDefrostErd,
      &numberOfFreshFoodDefrostsBeforeAFullDefrost);
}

static uint8_t NumberOfFreshFoodDefrosts(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfFreshFoodDefrosts;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsErd,
      &numberOfFreshFoodDefrosts);

   return numberOfFreshFoodDefrosts;
}

static uint8_t NumberOfFreshFoodDefrostsBeforeAFullDefrost(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfFreshFoodDefrostsBeforeAFullDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsBeforeAFullDefrostErd,
      &numberOfFreshFoodDefrostsBeforeAFullDefrost);

   return numberOfFreshFoodDefrostsBeforeAFullDefrost;
}

static void ResetNumberOfFreshFoodDefrostsToZero(NextDefrostTypeArbiter_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfFreshFoodDefrostsErd,
      clear);
}

static bool FreezerFilteredTemperatureTooWarmOnPowerUpIsSet(NextDefrostTypeArbiter_t *instance)
{
   bool FreezerFilteredTemperatureTooWarmOnPowerUpIsSet;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureTooWarmAtPowerUpErd,
      &FreezerFilteredTemperatureTooWarmOnPowerUpIsSet);

   return FreezerFilteredTemperatureTooWarmOnPowerUpIsSet;
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
            IncrementNumberOfFreshFoodDefrosts(instance);

            if(NumberOfFreshFoodDefrosts(instance) >= NumberOfFreshFoodDefrostsBeforeAFullDefrost(instance))
            {
               UpdateNextDefrostTypeTo(instance, DefrostType_Full);
            }
         }
         else if(currentDefrostType == DefrostType_Full)
         {
            ResetNumberOfFreshFoodDefrostsToZero(instance);
            UpdateNextDefrostTypeTo(instance, DefrostType_FreshFood);
         }
      }
   }
   else if((args->erd == instance->_private.config->enhancedSabbathModeErd) ||
      (args->erd == instance->_private.config->freezerDefrostWasAbnormalErd) ||
      (args->erd == instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd) ||
      (args->erd == instance->_private.config->convertibleCompartmentStateErd))
   {
      DetermineNumberOfFreshFoodDefrostsBeforeAFullDefrost(instance);
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

   DetermineNumberOfFreshFoodDefrostsBeforeAFullDefrost(instance);

   if(NumberOfFreshFoodDefrostsBeforeAFullDefrost(instance) == 0)
   {
      UpdateCurrentDefrostTypeTo(instance, DefrostType_Full);
      UpdateNextDefrostTypeTo(instance, DefrostType_Full);
   }
   else
   {
      if(FreezerFilteredTemperatureTooWarmOnPowerUpIsSet(instance))
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
