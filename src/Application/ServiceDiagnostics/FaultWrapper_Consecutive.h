/*!
 * @file
 * @brief Fault Wrapper
 * Implements the I_FaultWrapper interface for consecutive count faults
 * If the output fault isn't set, and the set request function is called
 * many times in a row without a clear request, set the fault.
 *
 * If the fault is set, and the clear request is called many times in a row, clear
 *
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef FAULTWRAPPER_CONSECUTIVE
#define FAULTWRAPPER_CONSECUTIVE

#include "I_FaultWrapper.h"
#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   uint8_t consecutiveRequestsToSetFault;
   uint8_t consecutiveRequestsToClearFault;
   Erd_t faultOutputErd;
} FaultWrapper_Consecutive_Configuration_t;

typedef struct
{
   I_FaultWrapper_t interface;

   struct
   {
      I_DataModel_t *dataModel;
      const FaultWrapper_Consecutive_Configuration_t *configuration;
      uint8_t consecutiveSetRequestCounts;
      uint8_t consecutiveClearRequestCounts;
   } _private;
} FaultWrapper_Consecutive_t;

void FaultWrapper_Consecutive_Init(
   FaultWrapper_Consecutive_t *instance,
   const FaultWrapper_Consecutive_Configuration_t *configuration,
   I_DataModel_t *dataModel);

#endif
