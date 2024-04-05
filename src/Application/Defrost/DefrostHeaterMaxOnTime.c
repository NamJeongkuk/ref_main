/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostHeaterMaxOnTime.h"
#include "ConvertibleCompartmentStateType.h"
#include "utils.h"
#include "PersonalityParametricData.h"

static bool ThereIsAConvertibleCompartment(DefrostHeaterMaxOnTime_t *instance)
{
   bool hasConvertibleCompartment;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->hasConvertibleCompartmentErd,
      &hasConvertibleCompartment);

   return hasConvertibleCompartment;
}

static void ResolveFreezerDefrostHeaterMaxOnTime(DefrostHeaterMaxOnTime_t *instance, bool freezerIsValid)
{
   if(freezerIsValid)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.heaterOnData->freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.heaterOnData->freezerHeater.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
   }
}

static void ResolveFreshFoodDefrostHeaterMaxOnTime(DefrostHeaterMaxOnTime_t *instance, bool freshFoodValid)
{
   if(freshFoodValid)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freshFoodDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.heaterOnData->freshFoodHeater.defrostHeaterMaxOnTimeInMinutes);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freshFoodDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.heaterOnData->freshFoodHeater.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
   }
}

static void ResolveConvertibleCompartmentDefrostHeaterMaxOnTime(
   DefrostHeaterMaxOnTime_t *instance,
   bool convertibleCompartmentIsValid,
   ConvertibleCompartmentStateType_t convertibleCompartmentState)
{
   if(convertibleCompartmentIsValid)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.heaterOnData->convertibleCompartmentHeater.defrostHeaterMaxOnTimeInMinutes);
   }
   else
   {
      if(convertibleCompartmentState == ConvertibleCompartmentStateType_Freezer)
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd,
            &instance->_private.heaterOnData->convertibleCompartmentHeaterAsFreezer.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
      }
      else
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd,
            &instance->_private.heaterOnData->convertibleCompartmentHeaterAsFreshFood.invalidThermistorDefrostHeaterMaxOnTimeInMinutes);
      }
   }
}

static void DataModelChanged(void *context, const void *_args)
{
   DefrostHeaterMaxOnTime_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;
   Erd_t erd = args->erd;

   if(erd == instance->_private.config->freezerEvaporatorThermistorIsValidResolvedErd)
   {
      const bool *isValid = args->data;
      ResolveFreezerDefrostHeaterMaxOnTime(instance, *isValid);
   }
   else if(erd == instance->_private.config->freshFoodEvaporatorThermistorIsValidResolvedErd)
   {
      const bool *isValid = args->data;
      ResolveFreshFoodDefrostHeaterMaxOnTime(instance, *isValid);
   }
   else if(ThereIsAConvertibleCompartment(instance) && erd == instance->_private.config->convertibleCompartmentEvaporatorThermistorIsValidResolvedErd)
   {
      const bool *isValid = args->data;
      ConvertibleCompartmentStateType_t convertibleCompartmentState;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->convertibleCompartmentStateErd,
         &convertibleCompartmentState);
      ResolveConvertibleCompartmentDefrostHeaterMaxOnTime(
         instance,
         *isValid,
         convertibleCompartmentState);
   }
   else if(ThereIsAConvertibleCompartment(instance) && erd == instance->_private.config->convertibleCompartmentStateErd)
   {
      const ConvertibleCompartmentStateType_t *convertibleCompartmentState = args->data;
      bool convertibleCompartmentIsValid;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->convertibleCompartmentEvaporatorThermistorIsValidResolvedErd,
         &convertibleCompartmentIsValid);
      ResolveConvertibleCompartmentDefrostHeaterMaxOnTime(
         instance,
         convertibleCompartmentIsValid,
         *convertibleCompartmentState);
   }
}

void DefrostHeaterMaxOnTime_Init(
   DefrostHeaterMaxOnTime_t *instance,
   I_DataModel_t *dataModel,
   const DefrostHeaterMaxOnTimeConfiguration_t *config,
   const DefrostHeaterOnData_t *heaterOnData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.heaterOnData = heaterOnData;

   EventSubscription_Init(
      &instance->_private.onDataModelChange,
      instance,
      DataModelChanged);
   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.onDataModelChange);

   bool freezerIsValid;
   bool freshFoodIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporatorThermistorIsValidResolvedErd,
      &freezerIsValid);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporatorThermistorIsValidResolvedErd,
      &freshFoodIsValid);

   ResolveFreezerDefrostHeaterMaxOnTime(instance, freezerIsValid);
   ResolveFreshFoodDefrostHeaterMaxOnTime(instance, freshFoodIsValid);

   if(ThereIsAConvertibleCompartment(instance))
   {
      bool convertibleCompartmentIsValid;
      ConvertibleCompartmentStateType_t convertibleCompartmentState;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->convertibleCompartmentEvaporatorThermistorIsValidResolvedErd,
         &convertibleCompartmentIsValid);
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->convertibleCompartmentStateErd,
         &convertibleCompartmentState);

      ResolveConvertibleCompartmentDefrostHeaterMaxOnTime(
         instance,
         convertibleCompartmentIsValid,
         convertibleCompartmentState);
   }
}
