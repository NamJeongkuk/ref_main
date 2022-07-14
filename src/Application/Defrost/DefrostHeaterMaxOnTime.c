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

static void ResolveFreezerDefrostHeaterMaxOnTime(DefrostHeaterMaxOnTime_t *instance, bool freezerIsValid)
{
   if(freezerIsValid)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.defrostParametricData->freezerDefrostHeaterMaxOnTimeInMinutes);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freezerDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.defrostParametricData->freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
   }
}

static void ResolveFreshFoodDefrostHeaterMaxOnTime(DefrostHeaterMaxOnTime_t *instance, bool freshFoodValid)
{
   if(freshFoodValid)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freshFoodDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.defrostParametricData->freshFoodDefrostHeaterMaxOnTimeInMinutes);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freshFoodDefrostHeaterMaxOnTimeInMinutesErd,
         &instance->_private.defrostParametricData->freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
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
         &instance->_private.defrostParametricData->convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes);
   }
   else
   {
      if(convertibleCompartmentState == ConvertibleCompartmentState_Freezer)
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd,
            &instance->_private.defrostParametricData->freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
      }
      else
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd,
            &instance->_private.defrostParametricData->freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes);
      }
   }
}

static void DataModelChanged(void *context, const void *_args)
{
   REINTERPRET(instance, context, DefrostHeaterMaxOnTime_t *);
   REINTERPRET(args, _args, const DataModelOnDataChangeArgs_t *);
   REINTERPRET(erd, args->erd, Erd_t);

   if(erd == instance->_private.config->freezerEvaporatorThermistorIsValidErd)
   {
      REINTERPRET(isValid, args->data, const bool *);
      ResolveFreezerDefrostHeaterMaxOnTime(instance, *isValid);
   }
   else if(erd == instance->_private.config->freshFoodEvaporatorThermistorIsValidErd)
   {
      REINTERPRET(isValid, args->data, const bool *);
      ResolveFreshFoodDefrostHeaterMaxOnTime(instance, *isValid);
   }
   else if(erd == instance->_private.config->convertibleCompartmentEvaporatorThermistorIsValidErd)
   {
      REINTERPRET(isValid, args->data, const bool *);
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
   else if(erd == instance->_private.config->convertibleCompartmentStateErd)
   {
      REINTERPRET(convertibleCompartmentState, args->data, const ConvertibleCompartmentStateType_t *);
      bool convertibleCompartmentIsValid;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->convertibleCompartmentEvaporatorThermistorIsValidErd,
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
   const DefrostHeaterMaxOnTimeConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;

   EventSubscription_Init(
      &instance->_private.onDataModelChange,
      instance,
      DataModelChanged);
   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.onDataModelChange);

   bool freezerIsValid;
   bool freshFoodIsValid;
   bool convertibleCompartmentIsValid;
   ConvertibleCompartmentStateType_t convertibleCompartmentState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerEvaporatorThermistorIsValidErd,
      &freezerIsValid);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvaporatorThermistorIsValidErd,
      &freshFoodIsValid);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentEvaporatorThermistorIsValidErd,
      &convertibleCompartmentIsValid);
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentStateErd,
      &convertibleCompartmentState);

   ResolveFreezerDefrostHeaterMaxOnTime(instance, freezerIsValid);
   ResolveFreshFoodDefrostHeaterMaxOnTime(instance, freshFoodIsValid);
   ResolveConvertibleCompartmentDefrostHeaterMaxOnTime(
      instance,
      convertibleCompartmentIsValid,
      convertibleCompartmentState);
}
