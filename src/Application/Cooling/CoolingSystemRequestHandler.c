/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CoolingSystemRequestHandler.h"
#include "Constants_Binary.h"
#include "VoteType.h"
#include "StackAllocator.h"
#include "uassert.h"
#include <stdlib.h>
#include "Constants_Time.h"

enum
{
   ClearValue = UINT8_MAX
};

typedef struct
{
   CoolingSystemRequestHandler_t *instance;
   const uint8_t index;
   size_t erdSize;
   Vote_t care;
} CoolingSystemRequestErdCallbackContext_t;

static void FillOffValueInCoolingSystemRequestVote(void *context, void *coolingSystemRequestVoteData)
{
   CoolingSystemRequestErdCallbackContext_t *coolingSystemRequestErdCallbackContext = context;
   CoolingSystemRequestHandler_t *instance = coolingSystemRequestErdCallbackContext->instance;

   switch(coolingSystemRequestErdCallbackContext->erdSize)
   {
      case sizeof(U8Vote_t): {
         U8Vote_t *u8Vote = coolingSystemRequestVoteData;
         u8Vote->value = instance->_private.config->coolingSystemRequestVoteList.pairs[coolingSystemRequestErdCallbackContext->index].offValue;
         u8Vote->vote = coolingSystemRequestErdCallbackContext->care;
      }
      break;

      case sizeof(U16Vote_t): {
         U16Vote_t *u16Vote = coolingSystemRequestVoteData;
         u16Vote->value = instance->_private.config->coolingSystemRequestVoteList.pairs[coolingSystemRequestErdCallbackContext->index].offValue;
         u16Vote->vote = coolingSystemRequestErdCallbackContext->care;
      }
      break;

      case sizeof(U32Vote_t): {
         U32Vote_t *u32Vote = coolingSystemRequestVoteData;
         u32Vote->value = instance->_private.config->coolingSystemRequestVoteList.pairs[coolingSystemRequestErdCallbackContext->index].offValue;
         u32Vote->vote = coolingSystemRequestErdCallbackContext->care;
      }
      break;

      default:
         uassert(!"Erd size not supported");
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->coolingSystemRequestVoteList.pairs[coolingSystemRequestErdCallbackContext->index].coolingSystemRequestVoteErd,
      coolingSystemRequestVoteData);
}

static void VoteOffForAllTheLoadsWithCareSetTo(CoolingSystemRequestHandler_t *instance, Vote_t care)
{
   for(uint8_t erdIndex = 0; erdIndex < instance->_private.config->coolingSystemRequestVoteList.numberOfPairs; erdIndex++)
   {
      uint8_t erdSize = DataModel_SizeOf(
         instance->_private.dataModel,
         instance->_private.config->coolingSystemRequestVoteList.pairs[erdIndex].coolingSystemRequestVoteErd);

      CoolingSystemRequestErdCallbackContext_t coolingSystemRequestErdCallbackContext = {
         instance,
         erdIndex,
         erdSize,
         care
      };

      StackAllocator_Allocate(erdSize, FillOffValueInCoolingSystemRequestVote, &coolingSystemRequestErdCallbackContext);
   }
}

static void ClearRequest(CoolingSystemRequestHandler_t *instance)
{
   uint8_t clearValue = ClearValue;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->requestErd,
      &clearValue);
}

static void SetDefrostDisabledTo(CoolingSystemRequestHandler_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->disableDefrostErd,
      &state);
}

static void CoolingSystemRequestHandlerTimeChanged(void *context, const void *args)
{
   CoolingSystemRequestHandler_t *instance = context;
   const uint8_t *offRequest = args;
   if(*offRequest != ClearValue)
   {
      if(*offRequest == true)
      {
         VoteOffForAllTheLoadsWithCareSetTo(instance, Vote_Care);
         SetDefrostDisabledTo(instance, true);
      }
      else if(*offRequest == false)
      {
         VoteOffForAllTheLoadsWithCareSetTo(instance, Vote_DontCare);
         SetDefrostDisabledTo(instance, false);
      }

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->statusErd,
         offRequest);

      ClearRequest(instance);
   }
}

static void VoteBasedOnStatus(CoolingSystemRequestHandler_t *instance)
{
   uint8_t coolingSystemOffStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->statusErd,
      &coolingSystemOffStatus);

   if(coolingSystemOffStatus)
   {
      VoteOffForAllTheLoadsWithCareSetTo(instance, Vote_Care);
      SetDefrostDisabledTo(instance, true);
   }
   else
   {
      VoteOffForAllTheLoadsWithCareSetTo(instance, Vote_DontCare);
      SetDefrostDisabledTo(instance, false);
   }
}

void CoolingSystemRequestHandler_Init(
   CoolingSystemRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const CoolingSystemRequestHandlerConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   VoteBasedOnStatus(instance);

   EventSubscription_Init(
      &instance->_private.coolingSystemRequestSubscription,
      instance,
      CoolingSystemRequestHandlerTimeChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->requestErd,
      &instance->_private.coolingSystemRequestSubscription);
}
