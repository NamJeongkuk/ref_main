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

static void IncrementNumberOfSecondaryOnlyDefrosts(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfSecondaryOnlyDefrosts;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsErd,
      &numberOfSecondaryOnlyDefrosts);

   numberOfSecondaryOnlyDefrosts++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsErd,
      &numberOfSecondaryOnlyDefrosts);
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

static void DetermineNumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfSecondaryOnlyDefrostsBeforeAFullDefrost;

   if(EnhancedSabbathModeIsEnabled(instance))
   {
      numberOfSecondaryOnlyDefrostsBeforeAFullDefrost =
         instance->_private.enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost;
   }
   else if(FreezerDefrostWasAbnormal(instance) || (ConvertibleCompartmentIsActingAsFreezerAndDefrostWasAbnormal(instance)))
   {
      numberOfSecondaryOnlyDefrostsBeforeAFullDefrost =
         instance->_private.defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet;
   }
   else
   {
      numberOfSecondaryOnlyDefrostsBeforeAFullDefrost =
         instance->_private.defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd,
      &numberOfSecondaryOnlyDefrostsBeforeAFullDefrost);
}

static uint8_t NumberOfSecondaryOnlyDefrosts(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfSecondaryOnlyDefrosts;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsErd,
      &numberOfSecondaryOnlyDefrosts);

   return numberOfSecondaryOnlyDefrosts;
}

static uint8_t NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(NextDefrostTypeArbiter_t *instance)
{
   uint8_t numberOfSecondaryOnlyDefrostsBeforeAFullDefrost;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd,
      &numberOfSecondaryOnlyDefrostsBeforeAFullDefrost);

   return numberOfSecondaryOnlyDefrostsBeforeAFullDefrost;
}

static void ResetNumberOfSecondaryOnlyDefrostsToZero(NextDefrostTypeArbiter_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->numberOfSecondaryOnlyDefrostsErd,
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
         if(currentDefrostType == DefrostType_SecondaryOnly)
         {
            IncrementNumberOfSecondaryOnlyDefrosts(instance);

            if(NumberOfSecondaryOnlyDefrosts(instance) >= NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(instance))
            {
               UpdateNextDefrostTypeTo(instance, DefrostType_Full);
            }
         }
         else if(currentDefrostType == DefrostType_Full)
         {
            ResetNumberOfSecondaryOnlyDefrostsToZero(instance);
            UpdateNextDefrostTypeTo(instance, DefrostType_SecondaryOnly);
         }
      }
   }
   else if((args->erd == instance->_private.config->enhancedSabbathModeErd) ||
      (args->erd == instance->_private.config->freezerDefrostWasAbnormalErd) ||
      (args->erd == instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd) ||
      (args->erd == instance->_private.config->convertibleCompartmentStateErd))
   {
      DetermineNumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(instance);
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

   DetermineNumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(instance);

   if(NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost(instance) == 0)
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
