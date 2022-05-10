/*!
 * @file
 * @brief Fault Wrapper
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FaultWrapper_Consecutive.h"
#include "utils.h"
#include "Constants_Binary.h"

static bool ConsecutiveSetRequestExceedLimit(FaultWrapper_Consecutive_t *instance)
{
   return ((++instance->_private.consecutiveSetRequestCounts) >= (instance->_private.configuration->consecutiveRequestsToSetFault));
}

static bool ConsecutiveClearRequestExceedLimit(FaultWrapper_Consecutive_t *instance)
{
   return ((++instance->_private.consecutiveClearRequestCounts) >= (instance->_private.configuration->consecutiveRequestsToClearFault));
}

static bool FaultIsSet(FaultWrapper_Consecutive_t *instance)
{
   bool faultIsSet;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->faultOutputErd,
      &faultIsSet);
   return faultIsSet;
}

static void SetFault(FaultWrapper_Consecutive_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->faultOutputErd,
      set);
}

static void ClearFault(FaultWrapper_Consecutive_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.configuration->faultOutputErd,
      clear);
}

static void SetRequest(I_FaultWrapper_t *interface)
{
   REINTERPRET(instance, interface, FaultWrapper_Consecutive_t *);

   if(FaultIsSet(instance))
   {
      instance->_private.consecutiveClearRequestCounts = 0;
   }
   else if(ConsecutiveSetRequestExceedLimit(instance))
   {
      SetFault(instance);
      instance->_private.consecutiveSetRequestCounts = 0;
   }
}

static void ClearRequest(I_FaultWrapper_t *interface)
{
   REINTERPRET(instance, interface, FaultWrapper_Consecutive_t *);
   if(!FaultIsSet(instance))
   {
      instance->_private.consecutiveSetRequestCounts = 0;
   }
   else if(ConsecutiveClearRequestExceedLimit(instance))
   {
      instance->_private.consecutiveClearRequestCounts = 0;
      ClearFault(instance);
   }
}

static void Reset(I_FaultWrapper_t *interface)
{
   REINTERPRET(instance, interface, FaultWrapper_Consecutive_t *);
   instance->_private.consecutiveClearRequestCounts = 0;
   instance->_private.consecutiveSetRequestCounts = 0;
   ClearFault(instance);
}

static const I_FaultWrapper_Api_t api = { SetRequest, ClearRequest, Reset };

void FaultWrapper_Consecutive_Init(
   FaultWrapper_Consecutive_t *instance,
   const FaultWrapper_Consecutive_Configuration_t *configuration,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->interface.api = &api;
   instance->_private.configuration = configuration;
}
