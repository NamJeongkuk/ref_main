/*!
 * @file
 * @brief Fault Wrapper count
 * A count fault wrapper will set the output fault if, out of the last totalRequestsToConsider times,
 * the client has requested to set the fault at least requestsToSetFault times.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef FAULTWRAPPER_COUNT
#define FAULTWRAPPER_COUNT

#include "I_FaultWrapper.h"
#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   uint8_t requestsToSetFault;
   uint8_t totalRequestsToConsider;
   Erd_t faultOutputErd;
} FaultWrapper_Count_Configuration_t;

typedef struct
{
   I_FaultWrapper_t interface;

   struct
   {
      I_DataModel_t *dataModel;
      const FaultWrapper_Count_Configuration_t *configuration;
      uint32_t bitQueue;
      uint8_t bitQueueIndex;
   } _private;
} FaultWrapper_Count_t;

void FaultWrapper_Count_Init(
   FaultWrapper_Count_t *instance,
   const FaultWrapper_Count_Configuration_t *configuration,
   I_DataModel_t *dataModel);

#endif
