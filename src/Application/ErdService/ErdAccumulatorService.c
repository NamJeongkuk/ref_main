/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ErdAccumulatorService.h"
#include "utils.h"
#include "uassert.h"

static void I64AccumulatorFunction(void *_instance, const void *data)
{
   ErdAccumulatorService_t *instance = _instance;

   int64_t cumulativeValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);

   cumulativeValue = TRUNCATE_UNSIGNED_ADDITION(cumulativeValue, *(const int64_t *)data, INT64_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);
}

static void U64AccumulatorFunction(void *_instance, const void *data)
{
   ErdAccumulatorService_t *instance = _instance;

   uint64_t cumulativeValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);

   cumulativeValue = TRUNCATE_UNSIGNED_ADDITION(cumulativeValue, *(const uint64_t *)data, UINT64_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);
}

static void I32AccumulatorFunction(void *_instance, const void *data)
{
   ErdAccumulatorService_t *instance = _instance;

   int32_t cumulativeValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);

   cumulativeValue = TRUNCATE_UNSIGNED_ADDITION(cumulativeValue, *(const int32_t *)data, INT32_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);
}

static void U32AccumulatorFunction(void *_instance, const void *data)
{
   ErdAccumulatorService_t *instance = _instance;

   uint32_t cumulativeValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);

   cumulativeValue = TRUNCATE_UNSIGNED_ADDITION(cumulativeValue, *(const uint32_t *)data, UINT32_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);
}

static void I16AccumulatorFunction(void *_instance, const void *data)
{
   ErdAccumulatorService_t *instance = _instance;

   int16_t cumulativeValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);

   cumulativeValue = TRUNCATE_UNSIGNED_ADDITION(cumulativeValue, *(const int16_t *)data, INT16_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);
}

static void U16AccumulatorFunction(void *_instance, const void *data)
{
   ErdAccumulatorService_t *instance = _instance;

   uint16_t cumulativeValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);

   cumulativeValue = TRUNCATE_UNSIGNED_ADDITION(cumulativeValue, *(const uint16_t *)data, UINT16_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);
}

static void I8AccumulatorFunction(void *_instance, const void *data)
{
   ErdAccumulatorService_t *instance = _instance;

   int8_t cumulativeValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);

   cumulativeValue = TRUNCATE_UNSIGNED_ADDITION(cumulativeValue, *(const int8_t *)data, INT8_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);
}

static void U8AccumulatorFunction(void *_instance, const void *data)
{
   ErdAccumulatorService_t *instance = _instance;

   uint8_t cumulativeValue;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);

   cumulativeValue = TRUNCATE_UNSIGNED_ADDITION(cumulativeValue, *(const uint8_t *)data, UINT8_MAX);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->cumulativeValueErd,
      &cumulativeValue);
}

static void DataModelChanged(void *context, const void *_args)
{
   ErdAccumulatorService_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   for(uint8_t i = 0; i < instance->_private.config->inputErdList.numberOfErds; i++)
   {
      if(args->erd == instance->_private.config->inputErdList.erds[i])
      {
         if(instance->_private.accumulatorServiceFunction)
         {
            instance->_private.accumulatorServiceFunction(instance, args->data);
            break;
         }
      }
   }
}

static void VerifyConfiguration(ErdAccumulatorService_t *instance, uint8_t integerWidth)
{
   const ErdAccumulatorServiceConfig_t *config = instance->_private.config;
   I_DataModel_t *dataModel = instance->_private.dataModel;

   if(DataModel_SizeOf(dataModel, config->cumulativeValueErd) != integerWidth)
   {
      uassert(!"Cumulative Erd does not match configuration size");
   }
   
   for(uint8_t i = 0; i < config->inputErdList.numberOfErds; i++)
   {
      if(DataModel_SizeOf(dataModel, config->inputErdList.erds[i]) != integerWidth)
      {
         uassert(!"Input Erd does not match configuration size");
      }
   }
}

void ErdAccumulatorService_Init(
   ErdAccumulatorService_t *instance,
   I_DataModel_t *dataModel,
   const ErdAccumulatorServiceConfig_t *config,
   uint8_t integerWidth,
   bool integerIsSigned)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.accumulatorServiceFunction = NULL;

   VerifyConfiguration(instance, integerWidth);

   switch(integerWidth)
   {
      case sizeof(int64_t):
         instance->_private.accumulatorServiceFunction = integerIsSigned ? I64AccumulatorFunction : U64AccumulatorFunction;
         break;

      case sizeof(int32_t):
         instance->_private.accumulatorServiceFunction = integerIsSigned ? I32AccumulatorFunction : U32AccumulatorFunction;
         break;

      case sizeof(int16_t):
         instance->_private.accumulatorServiceFunction = integerIsSigned ? I16AccumulatorFunction : U16AccumulatorFunction;
         break;

      case sizeof(int8_t):
         instance->_private.accumulatorServiceFunction = integerIsSigned ? I8AccumulatorFunction : U8AccumulatorFunction;
         break;
         
      default:
         uassert(!"Unsupported type!");
         break;
   }

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelChangedSubscription);
}
