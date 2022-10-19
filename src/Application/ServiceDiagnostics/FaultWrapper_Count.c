/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FaultWrapper_Count.h"
#include "utils.h"
#include "Constants_Binary.h"

static void SetFault(FaultWrapper_Count_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->faultOutputErd,
      set);
}

static void ClearFault(FaultWrapper_Count_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->faultOutputErd,
      clear);
}

static void IncrementBufferIndex(FaultWrapper_Count_t *instance)
{
   instance->_private.bitQueueIndex = (instance->_private.bitQueueIndex + 1) % (instance->_private.configuration->totalRequestsToConsider);
}

static uint8_t GetNumberFaultRequests(FaultWrapper_Count_t *instance)
{
   uint8_t activeFaultRequests = 0;
   uint32_t buffer = instance->_private.bitQueue;
   for(uint8_t i = 0; i < instance->_private.configuration->totalRequestsToConsider; i++)
   {
      if((1 << i) & buffer)
      {
         activeFaultRequests++;
      }
   }
   return activeFaultRequests;
}

static void SetFaultIfNecessary(FaultWrapper_Count_t *instance)
{
   if(GetNumberFaultRequests(instance) >=
      instance->_private.configuration->requestsToSetFault)
   {
      SetFault(instance);
   }
   else
   {
      ClearFault(instance);
   }
}

static void PushFaultToQueue(
   FaultWrapper_Count_t *instance,
   bool faultIsRequested)
{
   if(faultIsRequested)
   {
      BIT_SET(instance->_private.bitQueue, instance->_private.bitQueueIndex);
   }
   else
   {
      BIT_CLEAR(instance->_private.bitQueue, instance->_private.bitQueueIndex);
   }

   IncrementBufferIndex(instance);
   SetFaultIfNecessary(instance);
}

static void SetRequest(I_FaultWrapper_t *interface)
{
   REINTERPRET(instance, interface, FaultWrapper_Count_t *);

   PushFaultToQueue(instance, SET);
}

static void ResetFaultBitQueue(FaultWrapper_Count_t *instance)
{
   instance->_private.bitQueue = 0;
   instance->_private.bitQueueIndex = 0;
}

static void ClearRequest(I_FaultWrapper_t *interface)
{
   REINTERPRET(instance, interface, FaultWrapper_Count_t *);

   PushFaultToQueue(instance, CLEAR);
}

static void Reset(I_FaultWrapper_t *interface)
{
   REINTERPRET(instance, interface, FaultWrapper_Count_t *);

   ClearFault(instance);
   ResetFaultBitQueue(instance);
}

static const I_FaultWrapper_Api_t api = { SetRequest, ClearRequest, Reset };

/*!
 * @param instance
 * @param configuration
 * @param dataModel
 */
void FaultWrapper_Count_Init(
   FaultWrapper_Count_t *instance,
   const FaultWrapper_Count_Configuration_t *configuration,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.configuration = configuration;
   instance->interface.api = &api;
}
